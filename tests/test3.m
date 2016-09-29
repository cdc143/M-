/* Test AST and code generation of code with loops
   Computes the product of two positive inputted nums */
begin
input x; 
input y;
z:=0;
while x do
begin
  z:=z+y;
  x:=x-1;
end;
write z;
end

