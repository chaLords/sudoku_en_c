// Skeleton structure:

typedef struct {
    int xi_row, xi_col;
    int xj_row, xj_col;
} Arc;

typedef struct {
    Arc *arcs;
    int capacity;
    int size;
    int front;
    int rear;
} ArcQueue;

// Implement:
// 1. arc_queue_create()
// 2. arc_queue_push()
// 3. arc_queue_pop()
// 4. arc_queue_destroy()
// 5. ac3_revise_arc()
// 6. ac3_enforce_consistency()
// 7. ac3_propagate_from()
