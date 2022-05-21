{
   n = index($1,"_");
   print substr($1,1,n-1);
   # printf "[dinclude:]amiga39/fd/%s\t\t+amiga39\n",$1;
   # printf "[../include/]amiga39/fd/%s\t\t+amiga39\n",$1;
}
