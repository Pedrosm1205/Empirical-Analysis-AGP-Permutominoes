#include "DCEL.hpp"

using namespace std;

int main() {

  DCEL *d = new DCEL();

  // cout << "\n-----------------------------------\n\n";
  vector<vector<string>> regions = d->compute_visibility();

  // cout << "\n-----------------------------------\n\n";
  d->dfs_polygon(regions);

  return 0;
}
