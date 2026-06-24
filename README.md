# How to run
To build the executable, at the directory of the project, the following should suffice:

```cd src/build```
```cmake .. -DCMAKE_PREFIX_PATH=~/or-tools```
```cmake --build .```

## Run empirical analysis on a single polygon
At ```src``` directory, run:
```./build/EXE < data/data<id>_<number_vertices>/test_grid```

### Example of execution
```
./build/EXE < data/data5_10/test_grid 
Faces: 
Boundary:
1 1 -> 1 2
1 2 -> 3 2
3 2 -> 3 3
3 3 -> 3 4
3 4 -> 2 4
2 4 -> 2 5
2 5 -> 3 5
3 5 -> 4 5
4 5 -> 5 5
5 5 -> 5 4
5 4 -> 5 3
5 3 -> 4 3
4 3 -> 4 2
4 2 -> 4 1
4 1 -> 3 1
3 1 -> 1 1
End Boundary

Face 1:
1 2 -> 1 1
1 1 -> 3 1
3 1 -> 3 2
3 2 -> 1 2
Done Face 1

Face 2:
2 5 -> 2 4
2 4 -> 3 4
3 4 -> 3 5
3 5 -> 2 5
Done Face 2

Face 3:
3 2 -> 3 1
3 1 -> 4 1
4 1 -> 4 2
4 2 -> 3 2
Done Face 3

Face 4:
3 3 -> 3 2
3 2 -> 4 2
4 2 -> 4 3
4 3 -> 3 3
Done Face 4

Face 5:
3 4 -> 3 3
3 3 -> 4 3
4 3 -> 4 4
4 4 -> 3 4
Done Face 5

Face 6:
3 5 -> 3 4
3 4 -> 4 4
4 4 -> 4 5
4 5 -> 3 5
Done Face 6

Face 7:
4 4 -> 4 3
4 3 -> 5 3
5 3 -> 5 4
5 4 -> 4 4
Done Face 7

Face 8:
4 5 -> 4 4
4 4 -> 5 4
5 4 -> 5 5
5 5 -> 4 5
Done Face 8

----------------------------------
Removal 0: 
Optimal Number of Guards: 2

Changes at vertices: 
5 4 -> 5 5
Change visibility: 3 4
----------------------------------
Removal 1: 
Optimal Number of Guards: 2

Changes at vertices: 
Change visibility: 4 3
3 3 -> 3 4
----------------------------------
Removal 2: 
Optimal Number of Guards: 1

Changes at vertices: 
4 2 -> 4 3
New Vertex: 3 2
----------------------------------
Removal 3: 
Optimal Number of Guards: 1
```


## Run empirical analysis
At ```src``` directory, to conduct a new empirical analysis with the same method as the one conducted, run:
```./get_data```


## How to visualize graphically the data obtained
The graph visualizer uses the data generated during the empirical analysis, showing some plots and also prints the location of instances where a decrement of the size of the optimal vertex guard set happens. More particularly, it uses the ```info.txt``` files from each ```data/data<id>_<number_vertices>/``` directory. Make sure that numpy, matplotlib and scipy are installed. At ```src directory```, the following should suffice: 
```python3 graph_analysis_data.py```

