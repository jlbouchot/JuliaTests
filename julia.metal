formalism of julia is
   All ::= none map toplevel function curly linenumber float64 float32 float16 int64 int32 int16 int8 boolean ref block using vcat vect block return tuple call assign package;  --Add all operators

   vect -> All * ... ; -- + instead of * if at least one element
   call -> All + ... ; -- should be + : at least one element is required
   assign -> All All ;
   package -> All ;
   using -> All ; -- Verify
   ref -> All All ;
   block -> All * ...
   toplevel -> All * ... ;
   function -> All All ;
   curly -> All * ... ;
   return -> All ;
   toplevel -> All * ... ;
   tuple -> All * ... ;
   vcat -> All * ... ;
   hcat -> All * ... ;

   -- linenumber -> ; -- CONTINUE

   int64  -> implemented as STRING ;
   int32  -> implemented as INTEGER ; -- Might be a int32 or 16
   int16  -> implemented as STRING ;
   int8  -> implemented as STRING ;
   boolean  -> implemented as STRING ;
   float64  -> implemented as STRING ;
   float32  -> implemented as STRING ;
   float16  -> implemented as STRING ;

   none  -> ; -- Atom

end