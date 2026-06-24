#include <bits/stdc++.h>
#include <string>

#include "DCEL.hpp"
#include "solver.hpp"

using namespace std;

int DCEL::cross(Point a, Point b, Point c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool DCEL::isCCW(Point a, Point b, Point c) { return cross(a, b, c) > 0; }

int DCEL::distM(const Point &a, const Point &b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

bool contains_segment(vector<int> &a, vector<int> &b) {
  bool a_horizontal = (a[1] == a[3]);
  bool b_horizontal = (b[1] == b[3]);
  if (a_horizontal != b_horizontal) {
    return false;
  }
  if (a_horizontal) {
    if (a[1] != b[1] || b[0] < min(a[0], a[2]) || b[2] < min(a[0], a[2]) ||
        b[0] > max(a[0], a[2]) || b[2] > max(a[0], a[2])) {
      return false;
    }
    return true;
  }
  if (a[0] != b[0] || b[1] < min(a[1], a[3]) || b[3] < min(a[1], a[3]) ||
      b[1] > max(a[1], a[3]) || b[3] > max(a[1], a[3])) {
    return false;
  }
  return true;
}

DCEL::DCEL() {
  int n;
  cin >> n;

  nfaces = 0;

  vertices = vector<Point>(n);
  for (int i = 0; i < n; i++) {
    cin >> vertices[i].x >> vertices[i].y;
  }

  vector<vector<int>> inp;
  int xo, yo, xd, yd;
  while (cin >> xo) {
    cin >> yo >> xd >> yd;
    inp.push_back({xo, yo, xd, yd});
  }

  set<vector<int>> segs;
  for (int i = 0; i < (int)inp.size(); i++) {
    bool safe2add = true;
    for (int j = 0; j < (int)inp.size(); j++) {
      if ((inp[i][0] != inp[j][0] || inp[i][1] != inp[j][1] ||
           inp[i][2] != inp[j][2] || inp[i][3] != inp[j][3]) &&
          (inp[i][0] != inp[j][2] || inp[i][1] != inp[j][3] ||
           inp[i][2] != inp[j][0] || inp[i][3] != inp[j][1]) &&
          contains_segment(inp[i], inp[j])) {
        safe2add = false;
        break;
      }
    }
    if (safe2add) {
      segs.insert(inp[i]);
    }
  }

  set<vector<int>> created;
  map<Point, vector<Edge *>> out;
  for (vector<int> seg : segs) {
    int xo = seg[0], yo = seg[1], xd = seg[2], yd = seg[3];
    if (created.find({xo, yo, xd, yd}) != created.end())
      continue;

    Edge *e1 = new Edge(Point(xo, yo), nullptr);
    Edge *e2 = new Edge(Point(xd, yd), e1);
    e1->twin = e2;

    edges.push_back(e1);
    edges.push_back(e2);

    created.insert({xo, yo, xd, yd});
    created.insert({xd, yd, xo, yo});

    out[Point(xo, yo)].push_back(e1);
    out[Point(xd, yd)].push_back(e2);
  }

  cout << "Faces: \n";
  cout << "Boundary:\n";
  Edge *first = nullptr, *prev = nullptr;
  for (int i = 0; i < n; i++) {
    int x_min = min(vertices[i].x, vertices[(i + 1) % n].x);
    int x_max = max(vertices[i].x, vertices[(i + 1) % n].x);
    int y_min = min(vertices[i].y, vertices[(i + 1) % n].y);
    int y_max = max(vertices[i].y, vertices[(i + 1) % n].y);

    Point curr = vertices[i];
    while (curr != vertices[(i + 1) % n]) {
      for (Edge *edge : out[curr]) {
        Point d = edge->twin->origin;
        if (d.x >= x_min && d.x <= x_max && d.y >= y_min && d.y <= y_max &&
            distM(edge->twin->origin, vertices[(i + 1) % n]) <
                distM(curr, vertices[(i + 1) % n])) {
          curr = d;
          if (point_edge.find(edge->origin) == point_edge.end()) {
            point_edge[edge->origin] = edge;
          }
          edge->boundary = true;
          if (first == nullptr) {
            first = edge;
          } else {
            edge->prev = prev;
            prev->next = edge;
          }
          prev = edge;
          cout << edge->origin.x << " " << edge->origin.y << " -> "
               << edge->twin->origin.x << " " << edge->twin->origin.y << endl;
          break;
        }
      }
    }
  }

  if (prev != first) {
    first->prev = prev;
    prev->next = first;
  }

  cout << "End Boundary\n\n";

  int idx_face_prints = 1;
  for (Edge *edge : edges) {
    if (!edge->boundary && edge->face == nullptr) {
      if (point_edge.find(edge->origin) == point_edge.end()) {
        point_edge[edge->origin] = edge;
      }
      Face *new_face = new Face(edge);
      new_face->idx = nfaces;
      nfaces++;
      faces.push_back(new_face);
      edge->face = new_face;
      Edge *curr = edge;
      cout << "Face " << idx_face_prints << ":\n";
      do {
        Edge *next = nullptr;

        cout << curr->origin.x << " " << curr->origin.y << " -> "
             << curr->twin->origin.x << " " << curr->twin->origin.y << endl;

        for (Edge *adj : out[curr->twin->origin]) {
          if (isCCW(curr->origin, curr->twin->origin, adj->twin->origin)) {
            next = adj;
            break;
          }
          if (cross(curr->origin, curr->twin->origin, adj->twin->origin) == 0 &&
              !(curr->origin == adj->twin->origin)) {
            next = adj;
          }
        }
        curr->next = next;
        next->prev = curr;
        next->face = new_face;
        curr = next;

      } while (curr != edge);
      cout << "Done Face " << idx_face_prints << "\n\n";
      idx_face_prints++;
    }
  }

  cout << "----------------------------------" << endl;
}

bool DCEL::contains_face(vector<Face *> &visited, Face *face) {
  for (Face *f : visited) {
    if (*f == *face) {
      return true;
    }
  }
  return false;
}

void DCEL::dfs(Edge *edge, Face *face, vector<Face *> &visited, int &removal,
               list<Point> &iter_vertices, vector<vector<string>> &regions,
               vector<bool> &cover_region) {
  Edge *face_edge = face->edge;

  // colocar aresta inicial como horizontal. Objetivo ignorar arestas verticais
  // interiores
  while (face_edge->origin.y != face_edge->twin->origin.y) {
    face_edge = face_edge->next;
  }

  Edge *curr = face_edge;
  set<int> already_cover;
  do {
    visited.push_back(curr->face);
    already_cover.insert(curr->face->idx);
    if (curr->next->origin.y == curr->next->twin->origin.y ||
        curr->next->twin->boundary) {
      curr = curr->next;
    } else {
      curr = curr->next->twin->next;
    }
  } while (curr != face_edge);

  curr = face_edge;
  do {
    Edge *twin = curr->twin;
    if (!twin->boundary && !contains_face(visited, twin->face)) {
      dfs(twin, twin->face, visited, removal, iter_vertices, regions,
          cover_region);
    }
    if (curr->next->origin.y == curr->next->twin->origin.y ||
        curr->next->twin->boundary) {
      curr = curr->next;
    } else {
      curr = curr->next->twin->next;
    }
  } while (curr != face_edge);

  // send to solver

  set<string> guard_vertices;
  for (Point p : iter_vertices) {
    guard_vertices.insert(p.toString());
  }

  cout << "Removal " << removal - 1 << ": \n";
  int OPT = solve(regions, guard_vertices, cover_region);
  // get_all_solutions(regions, guard_vertices, cover_region, OPT);
  cout << endl;

  if (edge == nullptr)
    return;

  set<Point> face_points;
  curr = edge;
  do {
    face_points.insert(curr->origin);
    if (curr->next->origin.y == curr->next->twin->origin.y ||
        curr->next->twin->boundary) {
      curr = curr->next;
    } else {
      curr = curr->next->twin->next;
    }
  } while (curr != edge);

  curr = edge;
  // cout << curr->origin.x << " " << curr->origin.y << " " <<
  // curr->twin->origin.x
  //      << " " << curr->twin->origin.y << endl;
  while (!curr->next->twin->boundary && !curr->twin->prev->twin->boundary) {
    if (curr->next->origin.y == curr->next->twin->origin.y) {
      curr = curr->next;
    } else {
      curr = curr->next->twin->next;
    }
  }
  Point d = curr->next->origin;
  // cout << d.x << " " << d.y << endl;
  cout << "Changes at vertices: \n";
  if (curr->next->twin->origin.x == curr->next->twin->next->twin->origin.x) {
    cout << d.x << " " << d.y << " -> " << d.x << " "
         << curr->next->twin->origin.y << endl;
  } else {
    if (!curr->next->twin->boundary) {
      cout << "Change visibility: " << d.x << " " << d.y << endl;
    } else
      cout << "New Vertex: " << d.x << " " << d.y << endl;
  }

  curr = edge;
  while (!curr->prev->twin->boundary && !curr->twin->next->twin->boundary) {
    if (curr->prev->origin.y == curr->prev->twin->origin.y) {
      curr = curr->next;
    } else {
      curr = curr->prev->twin->prev;
    }
  }
  Point o = curr->origin;
  // cout << o.x << " " << o.y << endl;
  if (curr->origin.x == curr->prev->twin->prev->origin.x) {
    cout << o.x << " " << o.y << " -> " << o.x << " " << curr->prev->origin.y
         << endl;
  } else {
    if (!curr->prev->twin->boundary) {
      cout << "Change visibility: " << o.x << " " << o.y << endl;
    } else
      cout << "New Vertex: " << o.x << " " << o.y << endl;
  }

  for (auto it = iter_vertices.begin(); it != iter_vertices.end(); ++it) {
    if (it == --iter_vertices.end()) {
      int x_min, x_max, y_min, y_max;
      x_min = min(it->x, iter_vertices.front().x);
      x_max = max(it->x, iter_vertices.front().x);
      y_min = min(it->y, iter_vertices.front().y);
      y_max = max(it->y, iter_vertices.front().y);
      if (o != *it && o != iter_vertices.front() && o.x >= x_min &&
          o.x <= x_max && o.y >= y_min && o.y <= y_max) {
        iter_vertices.insert(next(it), o);
        break;
      }
      if (d != *it && d != iter_vertices.front() && d.x >= x_min &&
          d.x <= x_max && d.y >= y_min && d.y <= y_max) {
        iter_vertices.insert(next(it), d);
        break;
      }
    } else {
      int x_min, x_max, y_min, y_max;
      x_min = min(it->x, next(it)->x);
      x_max = max(it->x, next(it)->x);
      y_min = min(it->y, next(it)->y);
      y_max = max(it->y, next(it)->y);
      if (o != *it && o != *next(it) && o.x >= x_min && o.x <= x_max &&
          o.y >= y_min && o.y <= y_max) {
        iter_vertices.insert(next(it), o);
        break;
      }
      if (d != *it && d != *next(it) && d.x >= x_min && d.x <= x_max &&
          d.y >= y_min && d.y <= y_max) {
        iter_vertices.insert(next(it), d);
        break;
      }
    }
  }

  for (auto it = iter_vertices.begin(); it != iter_vertices.end();) {
    if (*it != o && *it != d && face_points.find(*it) != face_points.end())
      it = iter_vertices.erase(it);
    else
      ++it;
  }

  for (auto it = iter_vertices.begin(); it != iter_vertices.end();) {
    Point b, c;
    if (it == --iter_vertices.end()) {
      b = iter_vertices.front();
      c = *next(iter_vertices.begin());
    } else if (next(it) == --iter_vertices.end()) {
      b = iter_vertices.back();
      c = iter_vertices.front();
    } else {
      b = *next(it);
      c = *next(next(it));
    }

    if (cross(*it, b, c) == 0) {
      if (it == --iter_vertices.end())
        iter_vertices.pop_front();
      else
        it = iter_vertices.erase(++it);
      break;
    } else
      ++it;
  }

  ofstream file("removal" + to_string(removal));
  if (file.is_open()) {
    file << iter_vertices.size() << endl;
    for (Point p : iter_vertices) {
      file << p.x << " " << p.y << endl;
    }
    file.close();
  } else
    cout << "Unable to open file\n";

  for (int region : already_cover) {
    cover_region[region] = false;
  }

  removal++;

  cout << "----------------------------------" << endl;
}

void DCEL::dfs_polygon(vector<vector<string>> &regions) {
  vector<Face *> visited;
  int removal = 1;
  list<Point> iter_vertices;
  for (Point p : vertices) {
    iter_vertices.push_back(p);
  }
  vector<bool> cover_region(nfaces, true);
  dfs(nullptr, faces[0], visited, removal, iter_vertices, regions,
      cover_region);
}

vector<vector<string>> DCEL::compute_visibility() {
  /*
   * East 0001
   * West 1000
   * South 0100
   * North 0010
   */

  vector<vector<string>> regions(faces.size());
  // cout << "size: " << faces.size() << endl;

  for (auto &[point, edge] : point_edge) {
    for (Face *face : faces) {
      face->mask = 0;
    }
    // cout << "Visibility of Point: " << point.x << " " << point.y << endl;

    queue<Edge *> q;
    vector<Face *> visited;

    Edge *curr = edge;
    // cout << "Quadrants:\n";
    do {
      if (!curr->boundary) {
        /*
        cout << curr->origin.x << " ";
        cout << curr->origin.y << " ";
        cout << curr->twin->origin.x << " ";
        cout << curr->twin->origin.y << endl;
        */
        q.push(curr);
        visited.push_back(curr->face);
      }
      curr = curr->twin->next;
    } while (curr != edge);
    // cout << "End Quadrants\n";

    // cout << "Visible Regions:\n";
    while (!q.empty()) {
      Edge *edge_face = q.front();
      q.pop();
      Edge *curr = edge_face;
      regions[curr->face->idx].push_back(to_string(point.x) + " " +
                                         to_string(point.y));
      /*
      cout << curr->origin.x << " " << curr->origin.y << " "
           << curr->twin->origin.x << " " << curr->twin->origin.y << endl;
      */
      do {
        if (curr->twin->boundary) {
          if (curr->origin.x == curr->twin->origin.x &&
              curr->origin.y < curr->twin->origin.y) {
            curr->face->mask |= 1; // East
          } else if (curr->origin.x == curr->twin->origin.x &&
                     curr->origin.y > curr->twin->origin.y) {
            curr->face->mask |= (1 << 3); // West
          } else if (curr->origin.x > curr->twin->origin.x &&
                     curr->origin.y == curr->twin->origin.y) {
            curr->face->mask |= (1 << 1); // North
          } else if (curr->origin.x < curr->twin->origin.x &&
                     curr->origin.y == curr->twin->origin.y) {
            curr->face->mask |= (1 << 2); // South
          }
        }
        curr = curr->next;
      } while (curr != edge_face);
      // cout << "mask: " << curr->face->mask << endl;
      curr = edge_face;
      do {
        if (!curr->twin->boundary &&
            !((curr->origin.x == curr->twin->origin.x &&
               curr->origin.x == point.x) ||
              (curr->origin.y == curr->twin->origin.y &&
               curr->origin.y == point.y))) {
          curr->twin->face->mask |= curr->face->mask;
        }

        if (curr->origin.x == curr->twin->origin.x &&
            curr->origin.y < curr->twin->origin.y &&
            ((curr->face->mask & 1) == 1 || curr->origin.x == point.x)) {
          curr = curr->next; // East
          continue;
        } else if (curr->origin.x == curr->twin->origin.x &&
                   curr->origin.y > curr->twin->origin.y &&
                   (((curr->face->mask >> 3) & 1) == 1 ||
                    curr->origin.x == point.x)) {
          curr = curr->next; // West
          continue;
        } else if (curr->origin.x > curr->twin->origin.x &&
                   curr->origin.y == curr->twin->origin.y &&
                   (((curr->face->mask >> 1) & 1) == 1 ||
                    curr->origin.y == point.y)) {
          curr = curr->next; // North
          continue;
        } else if (curr->origin.x < curr->twin->origin.x &&
                   curr->origin.y == curr->twin->origin.y &&
                   (((curr->face->mask >> 2) & 1) == 1 ||
                    curr->origin.y == point.y)) {
          curr = curr->next; // South
          continue;
        }

        if (!curr->twin->boundary &&
            !contains_face(visited, curr->twin->face)) {
          q.push(curr->twin);
          visited.push_back(curr->twin->face);
        }
        curr = curr->next;
      } while (curr != edge_face);
    }
  }
  return regions;
}

/*
int main() {
  DCEL *d = new DCEL();
  cout << "Faces -> Point\n";
  for (DCEL::Face *face : d->faces) {
    DCEL::Point a = face->edge->origin;
    DCEL::Point b = face->edge->twin->origin;
    cout << a.x << " " << a.y << " " << b.x << " " << b.y << endl;
  }
  cout << "Done!\n";

  vector<vector<Point>> regions = d->compute_visibility();

  d->dfs_polygon(regions);

  return 0;
}
*/
