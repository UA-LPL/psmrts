#ifndef PsmrtsFormatModel_hpp
#define PsmrtsFormatModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>
#include <PsmrtsDataModel.hpp>

namespace psmrts {
  /**
   * @brief Abstract base class interface for shape model formats
   * 
   * This abstract class provides a generic interface to extractions of shape
   * model formats. This ABC should be the base class for all shape models that
   * can be used 
   * 
   * 
   */
  class PsmrtsFormatModel {
    public:

      typedef PsmrtsDataModel<int>       PsmrtsIndexData;
      typedef PsmrtsDataModel<double>    PsmrtsVectorData;

      PsmrtsFormatModel( ) { 
        m_format_name = "psmrts";
        m_format_type = "none";
      }

      PsmrtsFormatModel( const std::string &f_type, 
                         const std::string &f_name = "psmrts" ) { 
        m_format_name = f_name;
        m_format_type = f_type;
      }

      virtual ~PsmrtsFormatModel() { }

      /* Name of format system (PSMRTS) */
      virtual std::string format_model_type() const {
        return ( m_format_type );
      };

      /** Name of format model such as "dsk" and "obj" */
      virtual std::string format_model_name() const {
        return ( m_format_name );
      };

    /** Provide the source of the format data */
     virtual std::string format_model_source() const = 0;

      /** Unique tracer id of this instance */
      virtual std::string format_tracer_id()   const {
        return ( format_model_type() + "::" + format_model_name() + "::" + format_model_source() );
      };

      // These methods export the obj data as a mesh index and vector dataset
      virtual PsmrtsIndexData get_indexes()   const = 0;
      virtual PsmrtsVectorData get_vectors() const = 0;

    protected:
      std::string m_format_type;
      std::string m_format_name;

      inline void set_format_name( const std::string &name_t ) {
        m_format_name = name_t;
        return;
      }

      inline void set_format_type( const std::string &type_t ) {
        m_format_type = type_t;
        return;
      }

  };

} // namespace psmrts

#endif
