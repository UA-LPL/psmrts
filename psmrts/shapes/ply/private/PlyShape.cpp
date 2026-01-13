
#include <string>

#include "../PlyShape.hpp"
#include "PsmrtsPLYFormat.hpp"

namespace psmrts {
  PlyShape::PlyShape( const std::string &ply_file ) : PsmrtsProduct( ply_file, "ply"){
    PsmrtsPLYFormat m_model( ply_file );
    m_config = m_model.get_metadata(); // check if can rename to config
    m_mesh = m_model.get_mesh();
  }
} // namespace psmrts