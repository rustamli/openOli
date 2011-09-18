VAR
 n,c,i:integer;
BEGIN
 readln(n);
 for i:=1 to n do
   begin
     read(c);
     if odd(c) then write(c+2)
               else write(c);
     write(' ');
   end;
 writeln;
END. 
