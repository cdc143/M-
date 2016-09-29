/* Checking for grammer errors */
begin
input x;
if x then
x:=1; %Semicolon shouldn't be here
else
x:=(x-y; %Missing RPAR
end

