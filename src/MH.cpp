#include "header/MH.h"
//#include "header/utils.h"

#include <iostream>
#include <random>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <omp.h>

namespace py = pybind11;

void printProgressBar(int current, int total, int barLength = 100) {
    if (current == 0 || total == 0) return;

    double percentage = (static_cast<double>(current) / total) * 100;
    int filledLength = static_cast<int>((percentage / 100) * barLength);

    std::cout << "\rProgress: [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filledLength) {
            std::cout << "#";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << static_cast<int>(percentage) << "%";
    std::cout.flush();
}

MetropolisHastings::MetropolisHastings(std::function<double(std::vector<double>)> func)
    : func(func), engine(std::random_device{}()), normal(0.0, 1.0), uniform(0.0, 1.0) 
    {   
        //int numThreads = omp_get_max_threads();
        //omp_set_num_threads(numThreads);
        //std::cout << "Number of available CPU cores: " << numThreads << std::endl;
        stats.accepted = 0.0;
        stats.total = 0.0;
        stats.ratio = 0.0;
    }

py::dict MetropolisHastings::get_stats_history_dict() const {
    std::vector<double> accepted_vec, total_vec, ratio_vec;
    accepted_vec.reserve(stats_history.size());
    total_vec.reserve(stats_history.size());
    ratio_vec.reserve(stats_history.size());

    for (const auto& s : stats_history) {
        accepted_vec.push_back(s.accepted);
        total_vec.push_back(s.total);
        ratio_vec.push_back(s.ratio);
    }

    py::dict result;
    result["accepted"] = accepted_vec;
    result["total"] = total_vec;
    result["ratio"] = ratio_vec;

    return result;
}
std::vector<MHStats> MetropolisHastings::get_stats_history() const {
    return stats_history;
}
MHStats MetropolisHastings::get_stats() const {
    MHStats stats;
    stats.accepted = stats.accepted;
    stats.total = stats.total;
    stats.ratio = (stats.total > 0.0) ? (stats.accepted / stats.total) : 0.0;
    return stats;
}
std::vector<double>& MetropolisHastings::adaptProposal(std::vector<double>& std_devs) {
    int dim = std_devs.size();
    double alpha;
    if (stats.ratio < 0.2 || stats.accepted == 0.0) {
        alpha = 0.8; // increase learning rate when acceptance rate is low
    } else if (stats.ratio > 0.5) {
        alpha = 1.2; // decrease learning rate when acceptance rate is high
    } else {
        alpha = 1.0;
    }

    for (int d = 0; d < dim; d++) {
        std_devs[d] *= (alpha);
    }

    return std_devs;
}

std::vector<double> MetropolisHastings::iteration(std::vector<double> point, std::vector<double>& sigma, int steps) {
    int dim = point.size();
    std::vector<double> point_new = point;

    // Loop over iterations
    for (int d = 0; d < dim; d++) {
        //std::cout << "dim : "<< d << std::endl;
        point_new[d] += normal(engine) * sigma[d];
    }

    double r = std::min(1.0, func(point_new) - func(point));
    double u = std::log(uniform(engine));
    stats.total += 1.0;

    if (u <= r) 
    {   
        stats.accepted += 1.0;
        stats.ratio = stats.accepted / stats.total;
        return point_new;
    }
    else
    {   
        stats.ratio = stats.accepted / stats.total;
        return point;
    }
}

std::vector<std::vector<double>> MetropolisHastings::samples(int nsteps, std::vector<double> x0, std::vector<double> proposal_std, const int percent_step=1, bool adaptative=true, bool progressbar=true) {

    // Number of dimensions
    int dim = x0.size();
    std::vector<double> x = x0;
    std::vector<std::vector<double>> chain(nsteps, std::vector<double>(dim, 0.0));

    // Reset history at the start
    stats_history.clear();

    int next_percent = 0;

    // Loop over iterations
    for (int steps = 0; steps < nsteps; steps++) {
        
        int current_percent = (steps * 100) / nsteps;

        if (current_percent >= next_percent && progressbar == true) {
            printProgressBar(steps, nsteps);
            next_percent += percent_step;
        }
        
        x = iteration(x, proposal_std, steps);

        if (adaptative == true) {
            // Adapt proposal standard deviations based on acceptance rate
            proposal_std = adaptProposal(proposal_std);
        }
        
        // Store a copy of the current stats
        stats_history.push_back(stats);

        for (int d = 0; d < dim; d++) {
            chain[steps][d] = x[d];
        }   
    }

    return chain;
}

PYBIND11_MODULE(SamplerPy, m) {
    py::class_<MHStats>(m, "MHStats")
        .def_readonly("accepted", &MHStats::accepted)
        .def_readonly("total", &MHStats::total)
        .def_readonly("ratio", &MHStats::ratio);

    py::class_<MetropolisHastings>(m, "MetropolisHastings")
        .def(py::init<std::function<double(std::vector<double>)>>(), py::arg("func"))
        .def("samples", &MetropolisHastings::samples, py::arg("nsteps"), py::arg("x0"), py::arg("proposal_std"), py::arg("percent_step"), py::arg("adaptative"), py::arg("progressbar"))
        .def("iteration", &MetropolisHastings::iteration, py::arg("point"), py::arg("sigma"), py::arg("steps"))
        .def("adaptProposal", &MetropolisHastings::adaptProposal)
        .def("get_stats", &MetropolisHastings::get_stats)
        .def("get_stats_history", &MetropolisHastings::get_stats_history)
        .def("get_stats_history_dict", &MetropolisHastings::get_stats_history_dict);
}