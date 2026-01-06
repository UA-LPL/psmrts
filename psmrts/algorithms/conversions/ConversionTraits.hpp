#pragma once

#include <psmrts/core/PsmrtsUtilities.hpp>


namespace psmrts::algorithms::conversions {

  struct conversion_traits {
      inline static const size_t DigitsPrecision = 9;
      inline static const double DoubleTolerance = 1.0e-9;
      inline static const int JsonSpacing = -1;

    conversion_traits(  ) {
      digits_precision = DigitsPrecision;
      double_tolerance = DoubleTolerance;
      json_space       = JsonSpacing;
     }
    ~conversion_traits() = default;

    inline size_t digits() const {
      return ( digits_precision );
    }

    inline double tolerance() const {
      return ( double_tolerance );
    }    

    inline int spaces() const {
      return ( json_space );
    }

    size_t digits_precision;
    double double_tolerance;
    int json_space;
  };

  using ConversionTraits     = struct conversion_traits;


  struct conversion_parameters {
    conversion_parameters() { 
      index_t = 0;
      count_t = 1;
      get_all_t = false;
      traits_t = ConversionTraits( );
    }
    conversion_parameters(const size_t ith,
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t    = ith;
      count_t    = 1;
      get_all_t  = false;
      traits_t  = t;
    }       
    conversion_parameters(const size_t ith, const size_t n,
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t    = ith;
      count_t    = n;
      get_all_t  = false;
      traits_t  = t;
    }
 
    ~conversion_parameters() = default;

    static inline conversion_parameters get_all_values( const ConversionTraits &t = ConversionTraits()) {
      conversion_parameters parms( 0, 0);
      parms.get_all_t = true;
      parms.traits_t = t;
      return ( parms );
    }

    inline size_t index() const {
      return ( index_t );
    }

    inline size_t count() const {
      return ( count_t );
    }
    
    inline bool all() const {
      return ( get_all_t );
    }

    inline const ConversionTraits &traits() const {
      return ( traits_t  );
    }

    size_t index_t;
    size_t count_t;
    bool get_all_t;
    ConversionTraits traits_t;
  };

  using ConversionParameters = struct conversion_parameters;

} // namspace psmrts::algorithms::conversions