#include <iostream>
#include <iomanip>
//#include <vector>
//#include <queue>
//#include <stack>
#include <fstream>
#include <deque>
using namespace std;

enum tilestate { forward, backtrack };
class tile
{
   public:
      tile()
      {
         int i;
         x = -1; y = -1;
         value = 0;
         nodestate = forward;
         
         i_value = 0; // number of variables tried
      };

      tile operator++(int)
      {
         if(i_value < domain.size() )
         {
            i_value++;
            value = domain.front();// [i_value];
            domain.pop_front();
            domain.push_back(value);
            return *this;
         }
         //abort();
         value = -1;
         i_value = 0;
         nodestate = backtrack;
         return *this;
      };

      bool setValue();
      
      int x,y; // column 
      int value; // the value the tile assumes
      int i_value; // this is the iterator that goes through the possible domain values
      tilestate nodestate;
      deque<int> domain; //domain? 
      //deque<int> values_tried; //domain? or queue?
   private:
};


bool same_col(tile &lhs, tile &rhs);
bool same_row(tile &lhs, tile &rhs);
bool same_block(tile &lhs, tile &rhs);
bool remove_domain(tile &slot,int domain);


bool tile::setValue()
{
   if(domain.empty() || i_value >= domain.size())
      return false;

   value = domain.front(); //[i_value];
   return true;
}

class board
{
   public:
   board() {
      int i,j;
      for(i = 0;i < 9;i++)
         for(j = 0;j < 9;j++)
         {
            table[i][j] = 0;
         }
   };
   
   bool loadfile(const char *);
   void setTile(const tile& slot);
   void removeTile(const tile& slot);
   deque<tile> compute_unassigned();
   void remove_initial_constraints(deque<tile> &list);

   //queue<tile> unassigned;

   int table[9][9];
   
};

   
bool board::loadfile(const char *filename)
{
   int i = 0, j = 0;
   int cnt = 0;
   ifstream infile(filename);
   if(infile.fail())
      return false;

   //cout << "inserting ";
   while(!infile.eof())
   {
      if( i >= 9)
         break;
      if(j >= 9)
      {
         j = 0;
         i++;
      }
      infile >> table[i][j];  
      if(i == 8 && j == 8)
         return true;
      cnt++;
      //cout << table[i][j] << " ";
      j++;
   }
   //cout << "debug " << cnt << " values loaded\n";

   if(!infile.fail())
      return true;
   return false;
}

deque<tile> board::compute_unassigned()
{
   int i,j,k;
   deque<tile> unassigned;

   for(i=0;i < 9;i++)
   {
      for(j = 0;j< 9;j++)
      {
         tile newtile;
         if(table[i][j] != 0)
            continue;
   
         newtile.x = i;
         newtile.y = j;
         newtile.value = -1;



//onstraint propagation should eliminate the values from the domain
         for(k = 1;k < 10;k++)
         {
            newtile.domain.push_back(k);
         }

         unassigned.push_back(newtile);
         
      }
   }

   return unassigned;
}
ostream& operator << (ostream& os, const board& arg)
{
   int i,j;

   for(i = 0;i < 9;i++)
   {
      if(i%3 == 0)
      {
         for(j=0;j<9*4;j++)
            os << '-';
         os << endl;
      }

      for(j = 0;j < 9;j++)
      {
         if(j%3 == 0)
            os << " | ";
         if(arg.table[i][j] != 0)
            os << setw(3) << arg.table[i][j];
            //os << setw(3) << arg.table[i][j];
         else
            os << setw(3) << '-';
      }
      os << endl;
   }


   return os;
}

ostream& operator << (ostream& os, const tile& arg)
{
   deque<int>::const_iterator i = arg.domain.begin();
   os << "tile[" << arg.x << "][" << arg.y << "]";
#if 0
	os << " value " << arg.value;
#endif
   
//#if 1
   os << "  domain: [ ";
  for(;i != arg.domain.end();*i++)
      os << *i << " ";
   os << "]";

//#endif

#if 0
   os << "  i_value: " << arg.i_value;
#endif
   if(arg.nodestate == backtrack)
   {
      //os << "  backtracking";
   } else {
      //os << "  forward";
   }

  return os; 
}



void board::remove_initial_constraints(deque<tile> &list)
{
   int i,j;
   tile dummytile;
   deque<tile>::iterator next;
   
   for(next = list.begin();next != list.end();*next++)
   {
      for(i = 0;i < 9;i++)
      {
         for(j = 0;j < 9;j++)
         {
            if(i == (*next).x && j == (*next).y)
               continue;
            if(table[i][j] == 0)
               continue;

            dummytile.x = i;
            dummytile.y = j;
            if(same_col(*next,dummytile) || same_row(*next,dummytile) || same_block(*next,dummytile))
            {
               remove_domain(*next,table[i][j]);
               cout << "variables: " << *next << endl;
            }
         }
      }
   }
   
}

void board::setTile(const tile& slot)
{      
   int i,j;
   if((i = slot.x) == -1 || (j = slot.y) == -1)
      return; //not a valid tile

   table[i][j] = slot.value;
    
}

void board::removeTile(const tile& slot)
{
   int i,j;
   if((i = slot.x) == -1 || (j = slot.y) == -1)
      return; //not a valid tile

   table[i][j] = 0;
}



bool isSolution(board &bd, bool partial)
{
   int x,i,j,k,l,m;

   int tmp[10];

// check rows
   for(i=0;i< 9;i++)
   {
      for(x = 0;x < 10;x++)
         tmp[x] = 0;

      for(j=0;j<9;j++)
      {
         if(((m = bd.table[i][j]) == 0) && !partial)
            return false;
         if(tmp[m] == 0 || m == 0)
         {
            tmp[m] = 1;
         }else {
            return false;
         }
      }
      
   }
 
// check cols
   for(j=0;j< 9;j++)
   {
      for(x = 0;x < 10;x++)
         tmp[x] = 0;

      for(i=0;i<9;i++)
      {
         if((m = bd.table[i][j]) == 0 && !partial)
            return false;
         if(tmp[m] == 0 || m == 0)
         {
            tmp[m] = 1;
         }else {
            return false;
         }
      }
      
   }

//now check the 9 'blocks'

   for(k =0;k < 7;k += 3)
   {
      for(l = 0;l < 7;l += 3)
      {

         for(x = 0;x < 10;x++)
            tmp[x] = 0;

          for(i=k;i < k+3;i++)
            for(j=l;j < l+3;j++)
            {
               if((m = bd.table[i][j]) == 0 && !partial)
                  return false;
               if(tmp[m] != 0 && m != 0)
                  return false;
               tmp[m] = 1;
            } 
            

      }

   }


   return true;
}

bool isConsistent(board &bd)
{
}



bool remove_domain(tile &slot,int domain)
{
   int cnt = 0;
   deque<int>::iterator i = slot.domain.begin();

   for(;i != slot.domain.end();*i++)
   {
      cnt++;
      if(*i == domain)
      {
         i = slot.domain.erase(i);
         return true;
      }
   }
   return false;
}

bool add_domain(tile &slot,int domain)
{
   int cnt = 0;
   deque<int>::iterator negval = slot.domain.end();
   deque<int>::iterator i = slot.domain.begin();

   for(;i != slot.domain.end();*i++)
   { //first pass, see if the domain is already in their
      if(*i == domain)
         return false;
      else if(slot.i_value == cnt)
      {
         negval = i;
      }
   }


   //if(negval == slot.domain.end())
      //return false;
   slot.domain.push_front(domain);
   //slot.domain.insert(negval,domain);
   
   //cerr << "Inserting " << domain << " into ";
   //cerr << slot << endl;

   return true;
}


bool same_col(tile &lhs, tile &rhs)
{
   if(lhs.y == rhs.y)
   {
      if(lhs.x == rhs.x)
      {
         return false;
      }
   
      return true;
   }

   return false;
}


bool same_row(tile &lhs, tile &rhs)
{
   if(lhs.x == rhs.x)
   {
      if(lhs.y == rhs.y)
      {
         return false;
      }
   
      return true;
   }

   return false;
}

//not to be confused with checking to see if they are the same tile
bool same_block(tile &lhs, tile &rhs)
{
   int i,j;

   if(lhs.x == rhs.x && lhs.y == rhs.y)
      return false; //because that fails

   if ( (lhs.x/3)*3   == (rhs.x/3)*3 && (rhs.y/3)*3 == (lhs.y/3)*3)
      return true;

   return false;
}

void constrain(tile &slot,deque<tile> &list)
{
   deque<tile>::iterator i;
   int domain_alloc = slot.value;

   for(i = list.begin();i != list.end();*i++)
   {
      if(slot.x == (*i).x && slot.y == (*i).y)
         continue;

      if(same_row(slot,*i) || same_col(slot,*i) || same_block(slot,*i))
         if(remove_domain(*i,domain_alloc) == false)
         {
          //  cout << "failed remove " << slot.value <<" from "<< *i << endl;
         } else {
          //  cout << "removed " << slot.value <<" from "<< *i << endl;
         }
   }
}

void destrain(tile &slot,deque<tile> &list)
{
   deque<tile>::iterator i;
   int d = slot.value;
   
   for(i = list.begin();i != list.end();*i++)
   {
      if(same_row(slot,*i) || same_col(slot,*i) || same_block(slot,*i))
         if(add_domain(*i,d) == false)
         {
            //cout << "Failed to add " << d << " backto " << *i << endl;
         }
   }
}

void depth_first_search(board &bd)
{
   tile curr;
   bool found = false;
   deque<tile> assigned;
   deque<tile> unassigned;

   unassigned = bd.compute_unassigned();
   bd.remove_initial_constraints(unassigned);

   while(!isSolution(bd,false))
   {
      if(unassigned.empty())
      {
         cout << "no more values to assign\n";
         break;
      }
      curr = unassigned.front();
      unassigned.pop_front();


      if(!curr.setValue()) //assign first value
      {
         //cout << "incrimenting " << curr << endl;
         curr++;
         //cout << "incrimented " << curr << endl;

         unassigned.push_front(curr);

        if(!assigned.empty())
        {
            curr = assigned.front();
            assigned.pop_front();
            bd.removeTile(curr);
            //cout << "backtracking " << curr << endl;
            destrain(curr,unassigned);
            //destrain(curr,assigned);
            //cout << "parent incrimenting " << curr << endl;
            curr++;
            //cout << "parent incrimented " << curr << endl;
            unassigned.push_front(curr);
            continue;
        }
     }
      //cout << "Trying " << curr << endl;
      bd.setTile(curr);
      if(!isSolution(bd,true))
      {
         //value was not set correctly
         //cout << "didn't work " << curr << endl;
         bd.removeTile(curr);
         //destrain(curr,unassigned);
         //destrain(curr,assigned);
         //cout << "node incrimenting " << curr << endl;
         curr++; // check if tile has checked all possible values, then backtrack?
         //cout << "node incrimented " << curr << endl;

         unassigned.push_front(curr);
      } else {
         //cout << "Inserted: " << curr << endl;
         constrain(curr,unassigned);
         assigned.push_front(curr);
       //  cout << bd << endl;
         
      }
      //usleep(500000);
   }
   

   //fill 'unassigned variables' (boardpositions that are 0's)	
   //inservt values into fringe
   //fill variable
   // check if board consistent
   // if not, delete variable, backup, and try again
}

int main(int argc, char **argv)
{
   board bd;



   if(argc < 2 || argv[1] == NULL || argv[1] == '\0')
   {
   if(!bd.loadfile("evil"))
   {
      cerr << "failed to load file\n";
      cout << bd << endl;
      return -1;
   } 
   } else {
   if(!bd.loadfile(argv[1]))
   {
      cerr << "failed to load file " << argv[1] << endl;
      cout << bd << endl;
      return -1;
   } 
   }

   cout << bd;
   depth_first_search(bd);
   cout << bd;

   if(isSolution(bd,false))
      cout << "is a solution\n";
   else 
      cout << "is not a solution\n";


   return 0;
}
