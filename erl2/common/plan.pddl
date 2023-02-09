Number of literals: 13
Constructing lookup tables: [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%]
Post filtering unreachable actions:  [10%] [20%] [30%] [40%] [50%] [60%] [70%] [80%] [90%] [100%] [110%] [120%] [130%] [140%]
Have identified that bigger values of (charge kenny) are preferable
[01;34mNo analytic limits found, not considering limit effects of goal-only operators[00m
96% of the ground temporal actions in this problem are compression-safe
Initial heuristic = 7.000
b (6.000 | 60.000)b (4.000 | 90.001)b (3.000 | 280.004)b (2.000 | 310.005)
Resorting to best-first search
b (6.000 | 60.000)b (5.000 | 120.001)b (4.000 | 180.002)b (3.000 | 240.003)b (2.000 | 270.004);;;; Solution Found
; States evaluated: 71
; Cost: 270.004
; Time 0.05
0.000: (goto_waypoint kenny wp3 wp1)  [60.000]
60.001: (goto_waypoint kenny wp1 wp2)  [60.000]
120.002: (goto_waypoint kenny wp2 wp4)  [60.000]
180.003: (goto_waypoint kenny wp4 wp0)  [60.000]
240.004: (dock kenny wp0)  [30.000]
