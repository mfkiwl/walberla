//======================================================================================================================
//
//  This file is part of waLBerla. waLBerla is free software: you can 
//  redistribute it and/or modify it under the terms of the GNU General Public
//  License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version.
//  
//  waLBerla is distributed in the hope that it will be useful, but WITHOUT 
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
//  for more details.
//  
//  You should have received a copy of the GNU General Public License along
//  with waLBerla (see COPYING.txt). If not, see <http://www.gnu.org/licenses/>.
//
//! \file PlaneTest.cpp
//! \ingroup core
//! \author Christian Godenschwager <christian.godenschwager@fau.de>
//
//======================================================================================================================

#include "core/debug/TestSubsystem.h"
#include "core/logging/Logging.h"
#include "core/math/Plane.h"
#include "core/math/Vector3.h"
#include "core/mpi/Environment.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <cmath>
#include <vector>


using namespace walberla;
using math::Plane;

template < typename scalar_t >
struct RandomPointGenerator
{
   typedef walberla::Vector3<scalar_t> vector_t;
   typedef boost::mt11213b RandomNumberEngine;
   typedef boost::normal_distribution<scalar_t> NormalDistribution;
   typedef boost::variate_generator< RandomNumberEngine, NormalDistribution > Generator;

   RandomPointGenerator( const vector_t & mu, const vector_t & sigma )
   {
      for( uint_t i = 0; i < 3; ++i )
      {
         RandomNumberEngine rne;
         rne.seed( numeric_cast< RandomNumberEngine::result_type >( i * 642573 ) );
         normalDistributions.push_back( Generator( rne, NormalDistribution( mu[i], sigma[i] ) ) );
      }
   }

   vector_t operator()()
   {
      return vector_t( normalDistributions[0](), normalDistributions[1](), normalDistributions[2]() );
   }

private:
   std::vector< Generator > normalDistributions;
};

void testIOStream( const Plane & p )
{
   std::stringstream ss;
   ss << p;
   WALBERLA_CHECK( ss.good() );
   Plane p2;
   ss >> p2;
   WALBERLA_CHECK( ss.good() );
   WALBERLA_CHECK_EQUAL( p, p2 );
}

void testStreamInput()
{
   {
      std::istringstream iss("(<1,2,3>,<4,5,6>)");
      Plane p;
      iss >> p;
      WALBERLA_CHECK( iss.good() );
      Plane p_ref( Plane::Vec3Real( real_t(1), real_t(2), real_t(3) ), Plane::Vec3Real( real_t(4), real_t(5), real_t(6) ) );
      WALBERLA_CHECK_EQUAL( p, p_ref );
   }
   {
      std::istringstream iss("(<1,2,3>,4)");
      Plane p;
      iss >> p;
      WALBERLA_CHECK( iss.good() );
      Plane p_ref( Plane::Vec3Real( real_t(1), real_t(2), real_t(3) ), real_t(4) );
      WALBERLA_CHECK_EQUAL( p, p_ref );
   }
}

int main(int argc, char * argv[])
{
   debug::enterTestMode();

   mpi::Environment mpiEnv( argc, argv );

   typedef Vector3<real_t> Vec3Real;

   Plane p( Vec3Real( real_t(0), real_t(0), real_t(0) ), Vec3Real( real_t(1), real_t(0), real_t(0) ) );

   testIOStream( p );

   for( real_t x(-10); x < real_t(11); x += real_t(0.25) )
   {
      Vec3Real v( x, real_t(0), real_t(0) );
      WALBERLA_CHECK_FLOAT_EQUAL( p.signedDistance( v ), x );
      WALBERLA_CHECK_FLOAT_EQUAL( p.distance( v ), std::fabs( x ) );
      WALBERLA_CHECK_EQUAL( p.signedDistance( v ) <= real_t(0), p.isInHalfSpace( v ) );
   }

   for( real_t x(-10); x < real_t(11); x += real_t(0.25) )
   {
      static const Vector3<real_t> ZERO_VECTOR;
      Plane pShifted( p );
      pShifted.shift( x );
      WALBERLA_CHECK_FLOAT_EQUAL( pShifted.signedDistance( ZERO_VECTOR ), -x );
      for( real_t f(-10); f < real_t(11); f += real_t(0.25) )
      {
         Plane pShiftedScaled( pShifted );
         pShiftedScaled.scale( f );
         WALBERLA_CHECK_FLOAT_EQUAL( pShiftedScaled.signedDistance( ZERO_VECTOR ), -x * f );
      }
   }

   RandomPointGenerator<real_t> rpg( Vec3Real(0,0,0), Vec3Real(100,100,100) );

   for( int i = 0; i < 1000; ++i )
   {
      Vec3Real p0 = rpg();
      Vec3Real p1 = rpg();
      Vec3Real p2 = rpg();

      if( (p0 - p1).sqrLength() < 1e-6 || (p0 - p2).sqrLength() < 1e-6 || (p2 - p1).sqrLength() < 1e-6 )
      {
         --i;
         continue;
      }

      Plane plane( p0, (p0 - p1) % (p0 - p2) );

      WALBERLA_CHECK              ( realIsEqual( plane.signedDistance( p0 ), real_t(0) ) );
      WALBERLA_CHECK              ( realIsEqual( plane.distance      ( p0 ), real_t(0) ) );
      WALBERLA_CHECK              ( realIsEqual( plane.signedDistance( p1 ), real_t(0) ) );
      WALBERLA_CHECK              ( realIsEqual( plane.distance      ( p1 ), real_t(0) ) );
      WALBERLA_CHECK              ( realIsEqual( plane.signedDistance( p2 ), real_t(0) ) );
      WALBERLA_CHECK              ( realIsEqual( plane.distance      ( p2 ), real_t(0) ) );
      WALBERLA_CHECK_GREATER_EQUAL( plane.distance( p0 ), real_t(0) );
      WALBERLA_CHECK_GREATER_EQUAL( plane.distance( p1 ), real_t(0) );
      WALBERLA_CHECK_GREATER_EQUAL( plane.distance( p2 ), real_t(0) );
   }

   testStreamInput();
}