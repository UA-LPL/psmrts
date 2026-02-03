/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

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
