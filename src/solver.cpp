#include <bits/stdc++.h>

#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_solver.h"
#include "ortools/sat/model.h"
#include "ortools/sat/sat_parameters.pb.h"

using namespace std;
using namespace operations_research::sat;

int solve(const vector<vector<string>> &regions_verts,
          set<string> &guard_vertices, vector<bool> &cover_region) {
  CpModelBuilder cp_model;

  int nregions = (int)regions_verts.size();
  map<string, BoolVar> var_verts;

  // constraints
  for (int i = 0; i < nregions; i++) {
    if (!cover_region[i]) {
      continue;
    }
    LinearExpr coverage;
    for (string vert : regions_verts[i]) {
      if (guard_vertices.find(vert) == guard_vertices.end()) {
        continue;
      }
      if (var_verts.find(vert) == var_verts.end()) {
        var_verts[vert] = cp_model.NewBoolVar().WithName(vert);
      }
      coverage += var_verts[vert];
    }
    cp_model.AddGreaterOrEqual(coverage, 1);
  }

  LinearExpr nguards;
  for (auto &[vert, var] : var_verts) {
    nguards += var;
  }
  cp_model.Minimize(nguards);

  const CpSolverResponse response = Solve(cp_model.Build());

  if (response.status() == CpSolverStatus::OPTIMAL) {
    cout << "Optimal Number of Guards: " << response.objective_value() << endl;
  } else {
    cout << "Check error\n";
  }

  return response.objective_value();
}

void get_all_solutions(const vector<vector<string>> &regions_verts,
                       set<string> &guard_vertices, vector<bool> &cover_region,
                       int OPT) {
  CpModelBuilder cp_model;

  int nregions = (int)regions_verts.size();
  map<string, BoolVar> var_verts;

  // constraints
  for (int i = 0; i < nregions; i++) {
    if (!cover_region[i]) {
      continue;
    }
    LinearExpr coverage;
    for (string vert : regions_verts[i]) {
      if (guard_vertices.find(vert) == guard_vertices.end()) {
        continue;
      }
      if (var_verts.find(vert) == var_verts.end()) {
        var_verts[vert] = cp_model.NewBoolVar().WithName(vert);
      }
      coverage += var_verts[vert];
    }
    cp_model.AddGreaterOrEqual(coverage, 1);
  }

  LinearExpr nguards;
  for (auto &[vert, var] : var_verts) {
    nguards += var;
  }
  cp_model.AddEquality(nguards, OPT);

  // enumerate all solutions
  Model model;

  int num_solutions = 0;
  model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse &r) {
    cout << "Solution " << num_solutions << ": \n";
    for (auto &[vert, var] : var_verts) {
      if (SolutionBooleanValue(r, var)) {
        cout << vert << endl;
      }
    }
    cout << endl;
    num_solutions++;
  }));

  // Tell the solver to enumerate all solutions.
  SatParameters parameters;
  parameters.set_enumerate_all_solutions(true);
  model.Add(NewSatParameters(parameters));
  const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);

  cout << "Number of solutions found: " << num_solutions << endl;
}
