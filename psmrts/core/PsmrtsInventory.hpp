#ifndef PsmrtsInventory_hpp
#define PsmrtsInventory_hpp
#pragma once

#include <exception>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>


namespace psmrts {

  /** 
   * @brief PSMRTS system inventories
   * 
   * This is a PSMRTS Product inventory container. This is intended to 
   * provide a set of products that can be used to describe a completely
   * configured and maintained keyword parameter, shape, tracer and
   * priority tracer product system.
   * 
   * Typically, priority is given to the existance of a priority tracer.
   * Priority tracers are created by the contents of the TracerInventory.
   * The TracerInventory contains products from the shape 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-09-03 Kris J. Becker  Original Version
   */
  class PsmrtsInventory {
    public:
      using UIDType         = PsmrtsUID::UIDType;
      using PsmrtsParameter = ProductOption;

      using ShapeInventory          = PsmrtsCache<UIDType, PsmrtsShape>;
      using TracerInventory         = PsmrtsCache<UIDType, PsmrtsTracer>;
      using PriorityTracerInventory = PsmrtsCache<UIDType, PsmrtsPriorityTracer>;
      using ParameterInventory      = PsmrtsCache<std::string, PsmrtsParameter>;

      PsmrtsInventory( ) : m_tracker(), m_name("product"), m_type("type"), 
                         m_product_id( PsmrtsUID::get_uid() ) { }
      PsmrtsInventory( const std::string &pname ):
                     m_tracker(), m_name( pname ), m_type( "product" ),      
                     m_product_id( PsmrtsUID::get_uid() ) { }
      PsmrtsInventory( const std::string &pname, 
                     const std::string &ptype ):
                     m_tracker(), m_name( pname ), m_type( ptype ),
                     m_product_id( PsmrtsUID::get_uid() ) { }

      virtual ~PsmrtsInventory() { }


      /** Returns the name of the product */
      inline const std::string &name() const {
        return ( m_name );
      }
      
      /** Returns the unique ID of the product */
      inline const UIDType &uid() const {
        return ( m_product_id );
      }
      
      /** Returns a timestamp since the product has been created */
      inline PsmrtsThreadSafeCounter timestamp() const {
        return ( m_tracker.clone() );
      }

    private:
      PsmrtsThreadSafeCounter m_tracker;
      
      std::string             m_name;
      std::string             m_type;
      UIDType                 m_product_id;
  };

} // namespace psmrts

#endif
