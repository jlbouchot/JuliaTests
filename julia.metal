definition of julia is
  abstract syntax
   ALL  ::= none map toplevel function curly linenumber float64 float32 float16 int64 int32 int16 int8 boolean ref block using vcat vect block return tuple call assign package ;  --Add all operators

   vect -> ALL * ... ; -- + instead of * if at least one element
   call -> ALL + ... ; -- should be + : at least one element is required
   assign -> ALL ALL ;
   package -> ALL ;
   using -> ALL ; -- Verify
   ref -> ALL ALL ;
   block -> ALL * ... ;
   toplevel -> ALL * ... ;
   function -> ALL ALL ;
   curly -> ALL * ... ;
   return -> ALL ;
   toplevel -> ALL * ... ;
   tuple -> ALL * ... ;
   vcat -> ALL * ... ;
   hcat -> ALL * ... ;

   -- linenumber -> ; -- CONTINUE

   int64  -> implemented as STRING ;
   int32  -> implemented as INTEGER ; -- Might be a int32 or 16
   int16  -> implemented as STRING ;
   int8  -> implemented as STRING ;
   boolean  -> implemented as STRING ;
   float64  -> implemented as STRING ;
   float32  -> implemented as STRING ;
   float16  -> implemented as STRING ;
   ident  -> implemented as STRING ;

   none  -> ; -- Atom

end definition
