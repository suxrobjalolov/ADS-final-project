#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;

// define a Point class to represent 2D points
class Point {
public:
    double x, y;
    Point() : x(0), y(0) {}
    Point(double x, double y) : x(x), y(y) {}
};

// define a BoundingBox class to represent rectangular regions
class BoundingBox {
public:
    double x1, y1, x2, y2;
    BoundingBox(double x1, double y1, double x2, double y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
    bool contains(const Point& p) const {
        return p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2;
    }
    bool intersects(const BoundingBox& other) const {
        return !(other.x1 > x2 || other.x2 < x1 || other.y1 > y2 || other.y2 < y1);
    }
};

// define a QuadTree class to represent the quad-tree data structure
class QuadTree {
public:
    BoundingBox boundary;
    int capacity;
    vector<Point> points;
    QuadTree *nw, *ne, *sw, *se;
    QuadTree(const BoundingBox& boundary, int capacity) : boundary(boundary), capacity(capacity), nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr) {}
    ~QuadTree() {
        delete nw;
        delete ne;
        delete sw;
        delete se;
    }
    bool insert(const Point& p) {
        if (!boundary.contains(p)) {
            return false;
        }
        if (points.size() < capacity) {
            points.push_back(p);
            return true;
        }
        if (nw == nullptr) {
            subdivide();
        }
        if (nw->insert(p)) return true;
        if (ne->insert(p)) return true;
        if (sw->insert(p)) return true;
        if (se->insert(p)) return true;
        return false;
    }
    void queryRange(const BoundingBox& range, vector<Point>& results) const {
        if (!boundary.intersects(range)) {
            return;
        }
        for (const Point& p : points) {
            if (range.contains(p)) {
                results.push_back(p);
            }
        }
        if (nw == nullptr) {
            return;
        }
        nw->queryRange(range, results);
        ne->queryRange(range, results);
        sw->queryRange(range, results);
        se->queryRange(range, results);
    }
private:
    void subdivide() {
        double xMid = (boundary.x1 + boundary.x2) / 2;
        double yMid = (boundary.y1 + boundary.y2) / 2;
        nw = new QuadTree(BoundingBox(boundary.x1, boundary.y1, xMid, yMid), capacity);
        ne = new QuadTree(BoundingBox(xMid, boundary.y1, boundary.x2, yMid), capacity);
        sw = new QuadTree(BoundingBox(boundary.x1, yMid, xMid, boundary.y2), capacity);
        se = new QuadTree(BoundingBox(xMid, yMid, boundary.x2, boundary.y2), capacity);
    }
};

int main() {
    // set the parameters for the experiment
    int num_points = 1000000;
    double min_coord = 0;
    double max_coord = 1000;
    int num_queries = 1000;
    double query_size = 10;

    // generate the data set
    vector<Point> data(num_points);
    for (int i = 0; i < num_points; i++) {
        double x = min_coord + (max_coord - min_coord) * rand() / RAND_MAX;
        double y = min_coord + (max_coord - min_coord) * rand() / RAND_MAX;
        data[i] = Point(x,y);
    }

    // build the index
    BoundingBox boundary(min_coord, min_coord, max_coord, max_coord);
    int capacity = 4; // maximum number of points per quad-tree node
    QuadTree qtree(boundary,capacity);
    
     for(int i=0;i<num_points;i++){
         qtree.insert(data[i]);
     }

     // run the queries and record the times
     vector<double> query_times(num_queries);
     for (int i = 0; i < num_queries; i++) {
         double x = min_coord + (max_coord - min_coord) * rand() / RAND_MAX;
         double y = min_coord + (max_coord - min_coord) * rand() / RAND_MAX;
         BoundingBox query_range(x - query_size / 2, y - query_size / 2, x + query_size / 2, y + query_size / 2);
         vector<Point> results;
         auto start_time = chrono::high_resolution_clock::now();
         qtree.queryRange(query_range, results);
         auto end_time = chrono::high_resolution_clock::now();
         auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
         query_times[i] = duration / 1e6;
     }

     // compute the average query time
     double total_time = 0;
     for (int i = 0; i < num_queries; i++) {
         total_time += query_times[i];
     }
     double avg_time = total_time / num_queries;

     // print the results
     cout << "Quad-tree average query time: " << avg_time << " seconds" << endl;

    return 0;
}
