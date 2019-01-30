int main(void)
{
  int __retres;
  int i;
  int x;
  i = 0;
  x = -2147483647 - 1;
  i = 0;
  while (i <= 2147483647) {
    if (i == 2147483647) printf("i = %d (x = %d)\n",i,x);
    else 
      if (i < 0) printf("i = %d (x = %d)\n",i,x);
    if (i % 2 == 0) 
      /*@ assert rte: signed_overflow: -2147483647 ≤ i; */
      x = - i;
    else 
      /*@ assert rte: signed_overflow: -2147483648 ≤ x+i; */
      /*@ assert rte: signed_overflow: x+i ≤ 2147483647; */
      x += i;
    if (i < 0) break;
    /*@ assert rte: signed_overflow: i+1 ≤ 2147483647; */
    i ++;
  }
  __retres = 0;
  return __retres;
}
