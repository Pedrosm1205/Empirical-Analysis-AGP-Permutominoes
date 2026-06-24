#pragma once

#include <bits/stdc++.h>

class DCEL {
public:
  struct Edge;
  struct Point {
    int x, y;

    Point() : x(0), y(0) {}

    Point(int a, int b) : x(a), y(b) {}

    bool operator==(const Point &o) const { return x == o.x && y == o.y; }

    bool operator!=(const Point &o) const { return x != o.x || y != o.y; }

    bool operator<(const Point &o) const {
      return x < o.x || (x == o.x && y < o.y);
    }

    std::string toString() {
      return std::to_string(x) + " " + std::to_string(y);
    }
  };
  struct Line {
    Point p, q;

    Line(Point a, Point b) : p(a), q(b) {}
  };
  struct Face {
    Edge *edge;
    int mask;
    int idx;

    bool operator==(const Face &o) const {
      return edge->origin == o.edge->origin &&
             edge->twin->origin == o.edge->twin->origin;
    }

    Face(Edge *e) : edge(e), mask(0), idx(-1) {}
  };
  struct Edge {
    Point origin;
    bool boundary;
    Edge *twin;
    Face *face;
    Edge *next;
    Edge *prev;

    bool operator==(const Edge &o) const {
      return origin == o.origin && twin->origin == o.twin->origin;
    }
    bool operator!=(const Edge &o) const {
      return origin != o.origin || twin->origin != o.twin->origin;
    }

    Edge(Point o, Edge *t)
        : origin(o), boundary(false), twin(t), face(nullptr), next(nullptr),
          prev(nullptr) {}
  };
  std::vector<Edge *> edges;
  std::vector<Point> vertices;
  std::vector<Face *> faces;
  std::map<Point, Edge *> point_edge;
  int nfaces;
  static int cross(Point, Point, Point);
  static bool isCCW(Point, Point, Point);
  static int distM(const Point &, const Point &);
  static bool contains_face(std::vector<Face *> &, Face *);
  DCEL();
  void dfs(Edge *, Face *, std::vector<Face *> &, int &, std::list<Point> &,
           std::vector<std::vector<std::string>> &, std::vector<bool> &);
  void dfs_polygon(std::vector<std::vector<std::string>> &);
  std::vector<std::vector<std::string>> compute_visibility();
};
