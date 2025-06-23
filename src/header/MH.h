#ifndef MH_H
#define MH_H

#include <random>
#include <vector>
#include <functional>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Structure to hold acceptance statistics
struct MHStats {
    double accepted = 0.0;
    double total = 0.0;
    double ratio = 0.0;

    // New members to store stddev values for each parameter across all samples
    std::vector<double> stddevs; // vector of stddev values for each parameter

};

class MetropolisHastings {
public:

    // Constructor: takes a function that returns log-probability for a vector point
    MetropolisHastings(std::function<double(std::vector<double>)> func);

    // Runs the sampler for nsteps starting from x0 with proposal std deviations
    std::vector<std::vector<double>> samples(int nsteps, std::vector<double> x0, std::vector<double> proposal_std, const int percent_step, bool adaptative, bool progressbar);

    // Performs a single Metropolis-Hastings step from current point
    std::vector<double> iteration(std::vector<double> point, std::vector<double>& sigma, int steps);
    std::vector<double>& adaptProposal(std::vector<double>& std_devs); // new function to adapt proposal standard deviations

    // Get current acceptance stats
    MHStats get_stats() const;

    // Get history of stats at each step
    std::vector<MHStats> get_stats_history() const;
    py::dict get_stats_history_dict() const;

    

private:

    //int adapt_interval = 10;

    std::function<double(std::vector<double>&)> func; // function that returns log-probability for a vector point

    // Random number generation
    std::mt19937 engine;
    std::normal_distribution<double> normal;
    std::uniform_real_distribution<double> uniform;

    // Tracking acceptance stats
    MHStats stats;
    std::vector<MHStats> stats_history;

    std::vector<double> accepted_per_dim;
    std::vector<double> total_per_dim;

    //void printProgressBar(int current, int total, int barLength = 100);

};

#endif // MH_H