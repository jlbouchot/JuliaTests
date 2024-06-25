static Operator juliaOperators[] = {
   {"_null_tree", 0, 0},
   {"vect", -1, 1},
   {"call", -1, 2},
   {"assign", 2, 3},
   {"package", -1, 4},
   {"using", -1, 5},
   {"ref", -1, 6},
   {"block", -1, 7},
   {"toplevel", -1, 8},
   {"function", -1, 9},
   {"curly", -1, 10},
   {"return", -1, 11},
   {"tuple", -1, 12},
   {"vcat", -1, 13},
   {"hcat", -1, 14},
   {"endOfList", 0, 15},
   {"int64", 0, 16},
   {"int32", 0, 17},
   {"int16", 0, 18},
   {"int8", 0, 19},
   {"boolean", 0, 20},
   {"float64", 0, 21},
   {"float32", 0, 22},
   {"float16", 0, 23},
   {"ident", 0, 24},
   {"none", 0, 25},
   {"meta", 0, 26},
   {"comment", 0, 27},
   {"comment_s", -1, 28}
} ;
static short juliaNullTreeCode = 25 ;
static short juliaCommentTreeCode = 27 ;
static short juliaCommentsTreeCode = 28 ;
static short juliaIdentTreeCode = 24 ;
