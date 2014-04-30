#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

#ifndef ONLINE_JUDGE
#include <cassert>
#define ASSERT( expr ) assert( expr )
#else
#define ASSERT( expr )
#endif

static const unsigned max_islands   = 1000;
static const unsigned max_terminals = 10;
static const unsigned max_ras       = 20;
static const unsigned max_conns     = 100000;
static const unsigned max_verts     = max_islands*max_terminals;
static const unsigned max_edges     = max_islands*max_terminals*(max_terminals - 1) + max_conns;
static const uint32_t time_oo       = std::numeric_limits<uint32_t>::max();

struct ra_type
{
   uint16_t xl, yd, xr, yu;
};

struct terminal_type
{
   std::string name;
   uint16_t isl;
   uint16_t x, y;
};

struct island_type
{
   std::string name;
   uint16_t w, h;
   uint16_t n_terms;
   uint16_t terms[max_terminals];
   uint16_t n_ras;
   ra_type ras[max_ras];
};

struct conn_type
{
   uint16_t isl0, term0;
   uint16_t isl1, term1;
   unsigned time;
};

struct adj_type
{
   uint16_t u;
   uint32_t time;
};

uint16_t      n_islands, n_verts;
uint32_t      n_conns, n_edges;
island_type   islands[max_islands];
terminal_type terms[max_verts];
conn_type     conns[max_conns];
uint32_t      adj_displs[max_verts + 1];
uint32_t      adj_cnts[max_verts];
adj_type      adjs[max_edges];
std::vector<bool> edge_ready;

void
read_restricted_area( ra_type& ra )
{
   std::cin >> ra.xl >> ra.yd >> ra.xr >> ra.yu;
}

void
read_terminal( terminal_type& term,
               uint16_t isl_idx )
{
   term.isl = isl_idx;
   std::cin >> term.name >> term.x >> term.y;
}

void
read_island( island_type& isl,
             uint16_t isl_idx )
{
   std::cin >> isl.name >> isl.w >> isl.h >> isl.n_terms;
   for( unsigned ii = 0; ii < isl.n_terms; ++ii )
   {
      read_terminal( terms[n_verts], isl_idx );
      isl.terms[ii] = n_verts++;
   }
   std::cin >> isl.n_ras;
   for( unsigned ii = 0; ii < isl.n_ras; ++ii )
      read_restricted_area( isl.ras[ii] );
}

uint16_t
find_terminal( uint16_t isl_idx,
               std::string const& name )
{
   island_type& isl = islands[isl_idx];
   for( uint16_t ii = 0; ii < isl.n_terms; ++ii )
   {
      if( terms[isl.terms[ii]].name == name )
         return ii;
   }
   ASSERT( 0 );
}

void
read_connection( conn_type& con,
                 std::unordered_map<std::string,uint16_t> const& isl_map )
{
   std::string nb1, nw1, nb2, nw2;
   std::cin >> nb1 >> nw1 >> nb2 >> nw2;
   con.isl0 = isl_map.at( nw1 );
   con.term0 = find_terminal( con.isl0, nb1 );
   con.isl1 = isl_map.at( nw2 );
   con.term1 = find_terminal( con.isl1, nb2 );
   std::cin >> con.time;
}

void
read_input()
{
   std::unordered_map<std::string,uint16_t> isl_map;
   std::cin >> n_islands;
   for( uint16_t ii = 0; ii < n_islands; ++ii )
   {
      read_island( islands[ii], ii );
      isl_map.emplace( islands[ii].name, ii );
   }
   std::cin >> n_conns;
   for( unsigned ii = 0; ii < n_conns; ++ii )
      read_connection( conns[ii], isl_map );
}

void
count_edges( uint16_t isl_idx )
{
   island_type& isl = islands[isl_idx];
   for( unsigned ii = 0; ii < isl.n_terms; ++ii )
      adj_displs[isl.terms[ii] + 1] += isl.n_terms*(isl.n_terms - 1) + 1;
}

void
add_conn_edge( uint16_t u,
               uint16_t v )
{
   uint32_t idx = adj_displs[u] + adj_cnts[u]++;
   adjs[idx] = { v, time_oo };
   edge_ready[idx] = true;
}

void
make_graph()
{
   std::fill( adj_displs, adj_displs + n_verts + 1, 0 );
   for( uint16_t ii = 0; ii < n_conns; ++ii )
   {
      count_edges( conns[ii].isl0 );
      count_edges( conns[ii].isl1 );
   }
   for( uint16_t ii = 1; ii <= 2*n_conns; ++ii )
      adj_displs[ii] += adj_displs[ii - 1];
   n_edges = adj_displs[n_verts];

   std::fill( adj_cnts, adj_cnts + n_verts, 0 );
   edge_ready.resize( n_edges );
   std::fill( edge_ready.begin(), edge_ready.end(), false );
   for( uint32_t ii = 0; ii < n_conns; ++ii )
   {
      uint16_t u = conns[ii].isl0;
      uint16_t v = conns[ii].isl1;
      add_conn_edge( u, v );
      add_conn_edge( v, u );
   }
}

#ifndef ONLINE_JUDGE

void
dump_graph( std::string const& fn )
{
   std::ofstream file( fn, std::ios::binary );
   ASSERT( file.good() );
   file.write( (char*)&n_islands, sizeof(n_islands) );
   file.write( (char*)&n_verts, sizeof(n_islands) );
   file.write( (char*)&n_edges, sizeof(n_islands) );
   file.write( (char*)islands, sizeof(islands) );
   file.write( (char*)terms, sizeof(terms) );
   file.write( (char*)adj_displs, sizeof(adj_displs) );
   file.write( (char*)adj_cnts, sizeof(adj_cnts) );
   file.write( (char*)adjs, sizeof(adjs) );
   ASSERT( file.good() );
}

#endif

void
test_case()
{
   read_input();
   make_graph();
   dump_graph( "dump.dat" );
}

int
main()
{
   unsigned T;
   std::cin >> T;
   for( unsigned ii = 0; ii < T; ++ii )
      test_case();
}
