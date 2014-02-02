#include "MpmSim/ShapeFunction.h"

using namespace Eigen;
using namespace MpmSim;

ShapeFunction::PointToGridIterator::PointToGridIterator( const ShapeFunction& shapeFunction, float gridH, const Eigen::Vector3f& gridOrigin ) :
	m_shapeFunction( shapeFunction ),
	m_gridH( gridH ),
	m_gridOrigin( gridOrigin ),
	m_diameter( 2 * shapeFunction.supportRadius() )
{
	for( int dim=0; dim < 3; ++dim )
	{
		m_w[dim].resize( m_diameter );
		m_dw[dim].resize( m_diameter );
	}
}

void ShapeFunction::PointToGridIterator::initialize( const Vector3f& p, bool computeDerivatives )
{
	int r = (int)m_w[0].size() / 2;
	m_pos.setZero();

	for( int dim=0; dim < 3; ++dim )
	{
		float fracDimPos = ( p[dim] - m_gridOrigin[dim] ) / m_gridH;
		int dimPos = (int)floor( fracDimPos );
		fracDimPos -= dimPos;
		int j;
		
		m_base[ dim ] = dimPos + 1 - r;
		
		j = 1-r;
		for( int i = 0; i < m_diameter; ++i, ++j )
		{
			m_w[dim][i] = m_shapeFunction.w( j + fracDimPos );
		}

		if( computeDerivatives )
		{
			j = 1-r;
			for( int i = 0; i < m_diameter; ++i, ++j )
			{
				m_dw[dim][i] = m_shapeFunction.dw( j + fracDimPos ) / m_gridH;
			}
		}
	}
}

bool ShapeFunction::PointToGridIterator::next()
{
	++m_pos[0];
	if( m_pos[0] >= m_diameter )
	{
		m_pos[0] = 0;
		++m_pos[1];
		
		if( m_pos[1] >= m_diameter )
		{
			m_pos[1] = 0;
			++m_pos[2];
		}
		if( m_pos[2] >= m_diameter )
		{
			return false;
		}
	}

	return true;
}

const Eigen::Vector3i& ShapeFunction::PointToGridIterator::gridPos() const
{
	return m_pos;
}

void ShapeFunction::PointToGridIterator::weightGrad( Eigen::Vector3f& g ) const
{
	g[0] = m_dw[0][ m_pos[0] ] *  m_w[1][ m_pos[1] ] *  m_w[2][ m_pos[2] ];
	g[1] =  m_w[0][ m_pos[0] ] * m_dw[1][ m_pos[1] ] *  m_w[2][ m_pos[2] ];
	g[2] =  m_w[0][ m_pos[0] ] *  m_w[1][ m_pos[1] ] * m_dw[2][ m_pos[2] ];
}

float ShapeFunction::PointToGridIterator::weight() const
{
	return m_w[0][ m_pos[0] ] * m_w[1][ m_pos[1] ] * m_w[2][ m_pos[2] ];
}
