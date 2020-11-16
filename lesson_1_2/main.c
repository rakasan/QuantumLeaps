
int main()
{
  int counter = 0;
  
  /* ########lesson 1###############
++counter;
    ++counter;
      ++counter;
        ++counter;
          ++counter;
            ++counter;
              ++counter;
                ++counter;8*/
  
  
  while(counter<21)
  {
    ++counter;
    if((counter & 1) !=0) 
    {
       /* Do something is odd */
    }
    else
    {
      /* Do something if even */
    }
  }
  
  return 0;
}
