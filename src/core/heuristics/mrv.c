// Implement:
SudokuPosition heuristic_mrv(const ConstraintNetwork *net) {
    Position best = {-1, -1};
    int min_domain = INT_MAX;
    
    int size = constraint_network_get_board_size(net);
    
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            int domain_size = constraint_network_domain_size(net, r, c);
            
            // Skip filled cells (domain size 1)
            if (domain_size <= 1) continue;
            
            if (domain_size < min_domain) {
                min_domain = domain_size;
                best.row = r;
                best.col = c;
            }
        }
    }
    
    return best;
}
