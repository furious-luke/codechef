#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <limits>

#ifndef ONLINE_JUDGE
#include <cassert>
#define ASSERT( expr ) assert( expr )
#define LOG( ... ) printf( __VA_ARGS__ )
#else
#define ASSERT( expr )
#define LOG( ... )
#endif

static const unsigned max_islands   = 1000;
static const unsigned max_terminals = 10;
static const unsigned max_ras       = 20;
static const unsigned max_conns     = 100000;
static const unsigned max_verts     = max_islands*max_terminals;
static const unsigned max_edges     = max_islands*max_terminals*(max_terminals - 1) + max_conns;
static const unsigned max_island_w  = 251;
static const unsigned max_island_h  = 251;
static const uint32_t time_oo       = std::numeric_limits<uint32_t>::max();
static const uint16_t term_oo       = std::numeric_limits<uint16_t>::max();

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
   bool ras_sorted;
};

struct conn_type
{
   uint16_t isl0, term0;
   uint16_t isl1, term1;
   unsigned time;
};

struct adj_type
{
   uint16_t v;
   uint32_t time;
};

struct front_type
{
   uint16_t u, pre;
   uint32_t cost;

   bool
   operator<( front_type const& other ) const
   {
      return cost > other.cost;
   }
};

struct segment_type
{
   std::array<uint8_t,2> crds;
   uint8_t depth;

   bool
   operator<( segment_type const& other ) const
   {
      return depth < other.depth;
   }
};

uint16_t      n_islands, n_verts;
uint16_t      start, end;
uint32_t      n_conns, n_edges;
island_type   islands[max_islands];
terminal_type terms[max_verts];
conn_type     conns[max_conns];
uint32_t      adj_displs[max_verts + 1];
uint32_t      adj_cnts[max_verts];
adj_type      adjs[max_edges];
uint32_t      n_ferries[max_verts];
uint16_t      progs[max_verts];
uint32_t      dists[max_verts];

uint16_t      isl_adj_displs[max_ras + max_terminals + 1];
uint16_t      isl_adj_cnts[max_ras + max_terminals];
adj_type      isl_adjs[(max_ras + max_terminals)*(3*max_ras + max_terminals)];
uint16_t      isl_dists[max_ras + max_terminals];
uint16_t      isl_progs[max_ras + max_terminals];
segment_type  h_segs[max_islands][2*max_ras];
segment_type  v_segs[max_islands][2*max_ras];
std::unordered_map<uint16_t,std::list<std::array<uint8_t,2>>> paths;

void
read_restricted_area( uint16_t isl_idx,
                      uint8_t ra_idx )
{
   uint16_t xl, yd, xr, yu;
   std::cin >> xl >> yd >> xr >> yu;
   LOG( " Read restricted area: %d,%d,%d,%d\n", xl, yd, xr, yu );
   h_segs[isl_idx][ra_idx     ] = { { (uint8_t)xl, (uint8_t)xr }, (uint8_t)yd };
   h_segs[isl_idx][ra_idx + 10] = { { (uint8_t)xl, (uint8_t)xr }, (uint8_t)yu };
   v_segs[isl_idx][ra_idx     ] = { { (uint8_t)yd, (uint8_t)yu }, (uint8_t)xl };
   v_segs[isl_idx][ra_idx + 10] = { { (uint8_t)yd, (uint8_t)yu }, (uint8_t)xr };
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
   ASSERT( isl.w < max_island_w );
   ASSERT( isl.h < max_island_h );
   LOG( "Island: %s,%d,%d\n", isl.name.c_str(), isl.w, isl.h );
   for( unsigned ii = 0; ii < isl.n_terms; ++ii )
   {
      read_terminal( terms[n_verts], isl_idx );
      isl.terms[ii] = n_verts++;
   }
   std::cin >> isl.n_ras;
   LOG( " Number of restricted areas: %d\n", isl.n_ras );
   for( unsigned ii = 0; ii < isl.n_ras; ++ii )
      read_restricted_area( isl_idx, ii );
   isl.ras_sorted = false;
}

uint16_t
find_terminal( uint16_t isl_idx,
               std::string const& name )
{
   island_type& isl = islands[isl_idx];
   for( uint16_t ii = 0; ii < isl.n_terms; ++ii )
   {
      if( terms[isl.terms[ii]].name == name )
         return isl.terms[ii];
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
   LOG( "Connection: %s,%s,%s,%s\n", terms[con.term0].name.c_str(),
        islands[con.isl0].name.c_str(), terms[con.term1].name.c_str(),
        islands[con.isl1].name.c_str() );
}

void
read_input()
{
   std::unordered_map<std::string,uint16_t> isl_map;

   std::cin >> n_islands;
   LOG( "Number of islands: %d\n", n_islands );
   n_verts = 0;
   for( uint16_t ii = 0; ii < n_islands; ++ii )
   {
      read_island( islands[ii], ii );
      isl_map.emplace( islands[ii].name, ii );
   }

   std::cin >> n_conns;
   for( unsigned ii = 0; ii < n_conns; ++ii )
      read_connection( conns[ii], isl_map );

   std::string nbs, nws, nbc, nwc;
   std::cin >> nbs >> nws >> nbc >> nwc;
   start = find_terminal( isl_map[nws], nbs );
   end = find_terminal( isl_map[nwc], nbc );
}

void
count_terminal_edges( uint16_t isl_idx )
{
   island_type& isl = islands[isl_idx];
   uint32_t n_edges = isl.n_terms - 1;
   for( unsigned ii = 0; ii < isl.n_terms; ++ii )
      adj_displs[isl.terms[ii] + 1] += n_edges;
}

void
add_conn_edge( uint16_t u,
               uint16_t v,
               uint32_t time )
{
   uint32_t idx = adj_displs[u] + adj_cnts[u]++;
   adjs[idx] = { v, time };
   ++n_ferries[u];
}

void
make_graph()
{
   std::fill( adj_displs, adj_displs + n_verts + 1, 0 );
   for( uint16_t ii = 0; ii < n_islands; ++ii )
      count_terminal_edges( ii );
   for( uint32_t ii = 0; ii < n_conns; ++ii )
   {
      ++adj_displs[conns[ii].term0 + 1];
      ++adj_displs[conns[ii].term1 + 1];
   }
   for( uint16_t ii = 1; ii <= n_verts; ++ii )
      adj_displs[ii] += adj_displs[ii - 1];
   n_edges = adj_displs[n_verts];

   std::fill( adj_cnts, adj_cnts + n_verts, 0 );
   std::fill( n_ferries, n_ferries + n_verts, 0 );
   for( uint32_t ii = 0; ii < n_conns; ++ii )
   {
      uint16_t u = conns[ii].term0;
      uint16_t v = conns[ii].term1;
      add_conn_edge( u, v, conns[ii].time );
      add_conn_edge( v, u, conns[ii].time );
   }
   for( uint16_t ii = 0; ii < n_verts; ++ii )
   {
      island_type& isl = islands[terms[ii].isl];
      for( uint16_t jj = 0; jj < isl.n_terms; ++jj )
      {
         if( isl.terms[jj] == ii )
            continue;
         adjs[adj_displs[ii] + adj_cnts[ii]++] = { isl.terms[jj], time_oo };
      }
   }
}

template< int D >
inline
bool
line_seg_intersect( std::array<uint8_t,2> const& ss,
                    std::array<uint8_t,2> const& ff,
                    segment_type const& seg )
{
   if( ss[D] <= seg.depth && ff[D] >= seg.depth )
   {
      float dd = ff[D^1] - ss[D^1];
      float p = (float)ss[D^1] + dd*((float)ff[D] - (float)ss[D])/((float)seg.depth - (float)ss[D]);
      return (float)seg.crds[0] <= p && (float)seg.crds[1] >= p;
   }
   else
      return false;
}

bool
is_visible( uint16_t isl_idx,
            uint16_t aa,
            uint16_t bb )
{
   LOG( "   Checking visibility from %d to %d.\n", aa, bb );
   island_type const& isl = islands[isl_idx];
   std::array<uint8_t,2> ss, ff;
   if( aa < 4*isl.n_ras )
   {
      LOG( "    %d is an RA.\n" );
      ss = { h_segs[isl_idx][aa >> 1 + aa & 1].depth, h_segs[isl_idx][aa >> 1 + aa & 1].crds[0] };
   }
   else
   {
      LOG( "    %d is a terminal: \n", terms[isl.terms[aa - 4*isl.n_ras]].name.c_str() );
      ss = { (uint8_t)terms[isl.terms[aa - 4*isl.n_ras]].x, (uint8_t)terms[isl.terms[aa - 4*isl.n_ras]].y };
   }
   if( bb < 4*isl.n_ras )
      ff = { h_segs[isl_idx][bb >> 1 + bb & 1].depth, h_segs[isl_idx][bb >> 1 + bb & 1].crds[0] };
   else
      ff = { (uint8_t)terms[isl.terms[bb - 4*isl.n_ras]].x, (uint8_t)terms[isl.terms[bb - 4*isl.n_ras]].y };
   for( uint16_t ii = 0; ii < 2*isl.n_ras; ++ii )
   {
      if( ii == aa || ii == bb )
         continue;
      {
         segment_type const& seg = h_segs[isl_idx][ii];
         if( line_seg_intersect<0>( ss, ff, seg ) )
            return false;
      }
      {
         segment_type const& seg = v_segs[isl_idx][ii];
         if( line_seg_intersect<1>( ss, ff, seg ) )
            return false;
      }
   }
   return true;
}

void
make_island_graph( uint16_t term_idx )
{
   LOG( "  Making island graph.\n" );
   island_type const& isl = islands[terms[term_idx].isl];
   uint16_t n_isl_verts = 4*isl.n_ras + isl.n_terms;
   uint16_t max_adj = 3*isl.n_ras + isl.n_terms - 1;
   std::fill( isl_adj_cnts, isl_adj_cnts + n_isl_verts, 0 );
   for( uint16_t ii = 0; ii < n_isl_verts; ++ii )
   {
      // TODO: Make this better than O(N^2).
      for( uint16_t jj = 0; jj < n_isl_verts; ++jj )
      {
         if( jj == ii )
            continue;
         if( is_visible( terms[term_idx].isl, ii, jj ) )
            isl_adjs[ii*max_adj + isl_adj_cnts[ii]++] = { jj, time_oo };
      }
   }
}

uint32_t
island_traversal_time( uint16_t term_idx )
{
   make_island_graph( term_idx );

   // island_type const& isl = islands[isl_idx];
   // terminal_type const& term = terminals[term_idx];

   // std::cout << "Calculating traversal time for " << isl.name << ".\n";
   // std::cout << "  WxH = " << isl.w << "x" << isl.h << "\n";
   // if( !isl.ras_sorted )
   // {
   //    std::cout << "  Sorting RAs.\n";
   //    std::sort( &h_segs[isl_idx][0 ], &h_segs[isl_idx][isl.n_ras     ] );
   //    std::sort( &h_segs[isl_idx][10], &h_segs[isl_idx][isl.n_ras + 10] );
   //    std::sort( &v_segs[isl_idx][0 ], &v_segs[isl_idx][isl.n_ras     ] );
   //    std::sort( &v_segs[isl_idx][10], &v_segs[isl_idx][isl.n_ras + 10] );
   // }

   // for( uint16_t ii = 0; ii < isl.n_terms; ++ii )
   // {
   //    if( isl.terms[ii] == term_idx )
   //       continue;
   //    std::array<int16_t,2> ss{ terms[term_idx].x,      terms[term_idx].y };
   //    std::array<int16_t,2> ff{ terms[isl.terms[ii]].x, terms[isl.terms[ii]].y };
   //    std::array<int16_t,2> dd{ ff[0] - ss[0],          ff[1] - ss[1] };
   //    if( dd[0] > 0 )
   //    {
   //       // Check negative vertical segments.
   //       for( uint8_t jj = 0; jj < isl.n_ras; ++jj )
   //       {
   //          auto const& seg = v_segs[isl_idx][jj];
   //          if( ss[0] <= seg.depth && ff[0] >= seg.depth &&
   //              seg.crds[0] <= 
   //          {
   //             // Collision.
   //          }
   //       }
   //    }
   //    else if( dd[0] < 0 )
   //    {
   //       // Check positive vertical segments.
   //    }
   // }

   return 1000;
}

void
prepare_edge( uint16_t u,
              uint32_t edge )
{
   if( adjs[edge].time == time_oo )
   {
      LOG( "  Need to prepare edge.\n" );
      adjs[edge].time = island_traversal_time( u );
   }
#ifndef ONLINE_JUDGE
   else
      LOG( "  Edge already prepared.\n" );
#endif
}

void
dijkstra()
{
   std::fill( dists, dists + n_verts, time_oo );
   std::fill( progs, progs + n_verts, term_oo );
   std::priority_queue<front_type> Q;
   dists[start] = 0;
   Q.push( { start, term_oo, 0 } );

   while( !Q.empty() )
   {
      front_type front = Q.top(); Q.pop();
      uint16_t u = front.u;
      LOG( "Popped: %s\n", terms[u].name.c_str() );
      if( u == end )
      {
         LOG( " Finished.\n" );
         break;
      }

      std::array<uint32_t,2> rng{ adj_displs[u], 0 };

      // Did we arrive at this terminal by ferry?
      if( front.pre == term_oo || terms[u].isl != terms[front.pre].isl )
      {
         // Yes, arrived by ferry. Must scan all moves.
         rng[1] = adj_displs[u + 1];
         LOG( " Arrived by ferry.\n" );
      }
      else
      {
         // No, we arrived by foot. Only scan ferry options.
         rng[1] = adj_displs[u] + n_ferries[u];
         LOG( " Arrived by foot.\n" );
      }

      for( uint32_t ii = rng[0]; ii < rng[1]; ++ii )
      {
         uint16_t v = adjs[ii].v;
         LOG( " Checking connection to: %s\n", terms[v].name.c_str() );
         if( v == front.pre )
         {
            LOG( "  Don't backtrack to parent.\n" );
            continue;
         }
         prepare_edge( u, ii );
         uint32_t w = adjs[ii].time;
         LOG( "  Travel time: %d\n", w );
         if( dists[v] > dists[u] + w )
         {
            LOG( "  Updated.\n" );
            dists[v] = dists[u] + w;
            progs[v] = u;
            Q.push( { v, u, dists[v] } );
         }
      }
   }
}

void
write_path( uint16_t term )
{
   if( progs[term] != term_oo )
   {
      write_path( progs[term] );
      if( terms[progs[term]].isl == terms[term].isl )
      {
         std::cout << "PRINT ISLAND WALK\n";
      }
   }
   std::cout << terms[term].name << " " << islands[terms[term].isl].name << "\n";
}

void
test_case( unsigned tc )
{
   read_input();
   make_graph();

   dijkstra();

   LOG( "\n" );
   std::cout << "case " << tc << " Y\n";
   std::cout << dists[end] << "\n";
   write_path( end );
}

int
main()
{
   unsigned T;
   std::cin >> T;
   for( unsigned ii = 0; ii < T; ++ii )
   {
      test_case( ii );
   }
}
