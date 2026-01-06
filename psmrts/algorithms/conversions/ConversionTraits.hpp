#pragma once

#include <psmrts/core/PsmrtsUtilities.hpp>

namespace psmrts::algorithms::conversions {

/**
 * @brief Conversion traits to control formating and precision
 * 
 * This structure maintains defaults and allows user control of conversion
 * utilities that formulate access to the ProductOption object. This provides
 * consistent conversions to represent and test data of differing types. 
 * 
 * Convenience methods are provided to simply use.
 * 
 */
  struct conversion_traits {
      inline static const size_t DigitsPrecision = 9;
      inline static const double DoubleTolerance = 1.0e-9;
      inline static const int    JsonSpacing = -1;

    /** Default constructor */
    conversion_traits(  ) {
      digits_precision = DigitsPrecision;
      double_tolerance = DoubleTolerance;
      json_space       = JsonSpacing;
    }
     
    /** Specify digits and optional parameters */
    conversion_traits(const size_t digits,
                      const double tolerance = DoubleTolerance,
                      const int    spacing  = JsonSpacing ) {
      digits_precision = digits;
      double_tolerance = tolerance;
      json_space       = spacing;
     }
    conversion_traits( const double tolerance,
                       const size_t digits  = DigitsPrecision,
                       const int    spacing = JsonSpacing ) {
      digits_precision = digits;
      double_tolerance = tolerance;
      json_space       = spacing;
     }     

    ~conversion_traits() = default;

    /** Get digits precision for string formatting */
    inline size_t digits() const {
      return ( digits_precision );
    }

    inline double tolerance() const {
      return ( double_tolerance );
    }    

    inline int spaces() const {
      return ( json_space );
    }

    public:
      size_t digits_precision;
      double double_tolerance;
      int json_space;
  };

  using ConversionTraits     = struct conversion_traits;


  /**
   * @brief Structure maintains access control of extraction of ProductOption data
   * 
   * Users can specify the control of which values to extract from ProductOption
   * instances and how to establish default values representing conversion
   * failures. This allows easier methods for testing for and comparing data in
   * the PSMRTS system.
   * 
   * Convenience methods are provided to simply use.
   * 
   */
  struct conversion_parameters {
    // Enums for specify validity of occurances of default values
    using DefaultStatus  = enum { DefaultValid, DefaultInvalid };

    /** Default constructor for a single value, default traits */
    conversion_parameters() { 
      index_t        = 0;
      count_t        = 1;
      get_all_t      = false;
      default_status = DefaultValid;
      traits_t = ConversionTraits( );
    }
    /** Get a single value from an array */
    conversion_parameters(const size_t ith, 
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t        = ith;
      count_t        = 1;
      get_all_t      = false;
      default_status = DefaultValid;
      traits_t       = t;
    }    
    /** Get a single value from an array */
    conversion_parameters(const size_t ith, const DefaultStatus status,
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t        = ith;
      count_t        = 1;
      get_all_t      = DefaultValid;
      default_status = status;
      traits_t       = t;
    }
    /** Specify specific element in array and number of values to extract */       
    conversion_parameters(const size_t ith, const size_t n,
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t        = ith;
      count_t        = n;
      get_all_t      = false;
      default_status = DefaultValid;
      traits_t      = t;
    }
   /** Specify specific element in array and number of values to extract */       
    conversion_parameters(const size_t ith, const size_t n, 
                          const bool getall,
                          const DefaultStatus status = DefaultValid,
                          const ConversionTraits &t = ConversionTraits() ) {
      index_t        = ith;
      count_t        = n;
      get_all_t      = false;
      default_status = status;
      traits_t      = t;
    }    
 
    ~conversion_parameters() = default;

    /** Method constructor for getting all existing values with optional traits */
    static inline conversion_parameters get_all_values( const ConversionTraits &t = ConversionTraits()) {
      conversion_parameters parms( 0, 0);
      parms.get_all_t = true;
      parms.traits_t = t;
      return ( parms );
    }

    /** Returns 0-based index into array - 0 for scalar */
    inline size_t index() const {
      return ( index_t );
    }

    /** Number of values to extract - can expand scalars */
    inline size_t count() const {
      return ( count_t );
    }
    
    /** Returns status of full extraction flag - count ignored */
    inline bool all() const {
      return ( get_all_t );
    }

    /** Returns extractiom traits */
    inline const ConversionTraits &traits() const {
      return ( traits_t  );
    }

    public:
      size_t index_t;
      size_t count_t;
      bool get_all_t;
      DefaultStatus default_status;
      ConversionTraits traits_t;
  };

  using ConversionParameters = struct conversion_parameters;

} // namspace psmrts::algorithms::conversions