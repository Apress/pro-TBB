
__kernel void cl_print(global char *str) {
  for ( ; *str; ++str ) {
    printf("%c", *str);
    if(*str>96 && *str<123) *str-=32;
  }
}
