/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef Extractor_hpp
#define Extractor_hpp

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>

namespace psmrts::algorithms::conversions {


/**
   * @brief Extract template visitor for ProductOption like containers
   * 
   * This functor object will extract, converting if necessary, any of the
   * stored intrinsic types. This must be maintained alongside any changes
   * made to the ProductOption types and constraints that are added/removed.
   * 
   * Note that it is entirely possible to create a default value filled with
   * constant data where no extraction of data occurs but it fills the entire
   * array with default values.
   * 
   * Most any datatype can be converted to strings and compared (see Comparator)
   * to determine if a product configuration satisfies a product specification for
   * creation of data containers, such as PsmrtsShapes, and processes, such as
   * PsmrtsTracers.
   * 
   * This extractor supports customization of dataset conversions using the
   * ConversionTraits structs that determine digits of output for double
   * precision data to strings. It contains specifications of direct double
   * precision comparisons and formatting of JSON string output.
   * 
   * This example will construct an ProductOption of strings and extracts them
   * as integers.
   * 
   * @code {.C++}
   *   ProductOption radii( "radii", { 1, 2, 3 } );
   *   ProductOption::StringsVisitor(radii).get_all() == std::vector<std::string>({"1","2","3"}); 
   * code
   * @endcode
   * 
   * 
   * @author 2026-01-08 Kris J Becker
   */
  template <typename Container, typename Visitor>
    class Extractor {
      public:
        using Type        =  typename Visitor::Type;
        using TypeVector  =  typename Visitor::TypeVector;

        Extractor() : m_option(), 
                      m_traits( ConversionTraits() ), 
                      m_default( Visitor::TypeDefault ) { }
        Extractor( const Container &option ) {
          m_option  = option;
          m_traits  = ConversionTraits();
          m_default = Visitor::TypeDefault;
        }      
        Extractor( const Container &option,
                   const ConversionTraits &traits,
                   const Type default_v = Visitor::TypeDefault ) {
          m_option  = option;
          m_traits  = traits;
          m_default = default_v;
        }
        Extractor( const Container &option,
                   const Type default_v,
                   const ConversionTraits &traits = ConversionTraits() ) {
          m_option  = option;
          m_traits  = traits;
          m_default = default_v;
        }        
        virtual ~Extractor() = default;


        /** Name of the option dataset */
        inline std::string name() const {
          return ( m_option.name() );
        }

        /** Return reference to the current container */
        inline const Container &container() const {
          return ( m_option );
        }        

        /** Return the size of the options data set */
        inline size_t size() const {
          return ( m_option.size() );
        }
              
        /** Return the data default */
        static inline const Type &visitor_default( ) {
          return ( Visitor::TypeDefault );
        }

        /** Return the data default */
        inline const Type &default_value( ) const {
          return ( m_default );
        }

        /** Get values by index  */
        inline Type get( const size_t index = 0 ) const {
          ConversionParameters p( compute_range( index, 1, this->size() ) );
          TypeVector one;
          
          one.reserve( 1 );
          Visitor visitor( one, default_value(), p );
          m_option.visit( visitor );

          return ( one.front() );
        }

        /** Get the first value in the array  */
        inline Type front( ) const {
          ConversionParameters p( compute_range( 0, 1, this->size() ) );
          TypeVector one;
          
          one.reserve( 1 );
          Visitor visitor( one, default_value(), p );
          m_option.visit( visitor );

          return ( one.front() );
        }
        
        /** Get the last value in the array */
        inline Type back( ) const {
          size_t last_i = ( this->size() == 0 ) ? 0 : this->size() - 1;
          ConversionParameters p( compute_range( last_i, 1, this->size() ) );
          TypeVector one;
          
          one.reserve( 1 );
          Visitor visitor( one, default_value(), p );
          m_option.visit( visitor );

          return ( one.front() );
        }        
        
        /**
         * @brief Get the all data values and return in a user provided vector values
         * 
         * This method extracts value from a option potentially converting the
         * the type based upon the Visitor type. Some conversions may fail and
         * return a default value as specified in constructors (a reasonable
         * default is provided by the Visitor).
         * 
         * @param d  Vector<Type> is returned containing all the
         *             converted/extracted data with potential default/invalid
         *             elements.
         * @return const std::vector<Type>& Returns a reference to the return
         *             data vector
         */
        inline TypeVector &get_all( TypeVector&d ) const {
          ConversionParameters p = compute_range( 0, this->size(), this->size() );

          d.reserve( p.count() );
          Visitor visitor(  d, default_value(), p );
          m_option.visit( visitor );

          return ( d);
        }

        /** Extract data from container and return a vector of data
         * 
         * This method creates a local vector to data to extract the contents of
         * the stored container of the given visitor template type. Users can
         * specify a sub section of the array to extract. No parameters will
         * result in all values in the option to be converted, extracted and
         * returned to the caller.
         * 
         * Here is an example demonstrating how to fill a ProductOption (e) with
         * 10 values of the constant 1.0 (d).
         * 
         * @code
         *   auto d = ProductOption("d", 1.0);
         *   auto e = ProductOption(Extractor(d,d.get(0)).get_all(1,10));
         * @endcode
         *
         * @return TypeVector Vector of data extracted from option 
         */
        inline TypeVector get_all( const size_t index = 0,
                                   const size_t nvals = 0 ) const {

          ConversionParameters p = compute_range( index, nvals, this->size() );

          TypeVector d;
          d.reserve( p.count() );

          Visitor visitor(  d, default_value(), p );
          m_option.visit( visitor );

          return ( d);
        }        

        /**
         * @brief Get range of data from an array of option values
         * 
         * This method can be used to extract a range of data from an option.
         * 
         * @param d      Vector<Type> to place the values in. It will be resized
         *                 to the expected number of values to be returned by
         *                 this call.
         * @param index  Starting index of the data array to return.
         * @param nvals  Number of values to return. The computed range will
         *                 result in a max count of values, which is used to
         *                 resize the d vector for efficiency.
         * @return const std::vector<Type>& Reference to the returned data vector
         */
        inline const TypeVector &get_all( TypeVector &d, 
                                          const size_t index,
                                          const size_t nvals = 0 ) const {
          ConversionParameters p = compute_range( index, nvals, this->size() );
          
          d.reserve( p.count() );
          Visitor visitor(  d, default_value(), p );
          m_option.visit( visitor );

          return ( d );
        }


        /**
         * @brief Get range of data from an array of option values
         * 
         * This method can be used to extract a range of data from an option.
         * 
         * @param d      Vector<Type> to place the values in. It will be resized
         *                 to the expected number of values to be returned by
         *                 this call.
         * @param index  Starting index of the data array to return.
         * @param nvals  Number of values to return. The computed range will
         *                 result in a max count of values, which is used to
         *                 resize the d vector for efficiency.
         * @return const std::vector<Type>& Reference to the returned data vector
         */
        static inline Visitor create_visitor( TypeVector &d, const Container &c,
                                              const ConversionTraits &t = ConversionTraits(),
                                              const Type &default_v = Visitor::TypeDefault ) {

          Extractor e( c, t, default_v );
          ConversionParameters p  = e.compute_range( 0, c.size(), c.size() );
          d.reserve( c.size() );
          return ( Visitor ( d, default_v, p ) );
        }


        /**
         * @brief Compute a data extraction range parameter configuration
         * 
         * This method will compute a valid ConversionParameters struct that
         * contains a valid range for the given starting index, nvals and
         * max_size of the data vector extents. This parameter will be a valid
         * extractor for a given option with max_size.
         * 
         * @param index    Starting 0-based data vector index
         * @param nvals    Number of values to extract
         * @param max_size Intended to be the actual size of the data vector in
         *                   the dataset
         * @return ConversionParameters 
         */
        inline ConversionParameters compute_range( const size_t index, 
                                                   const size_t nvals,
                                                   const size_t max_size )
                                                   const {
          
          // Careful of cases where computations are negative!
          size_t n = nvals;
          if ( nvals == 0 ) {
            if ( index < max_size ) {
              n = max_size - index;
            }
          }
          return ( ConversionParameters ( index, n, traits() ) );
        }

        /** Return the conversion/extractio traits */
        inline const ConversionTraits &traits() const {
          return ( m_traits );
        }

      private:
        Container        m_option;
        ConversionTraits m_traits;
        Type             m_default;
  };  

}    // namespace psmrts::algoriths::conversions

#endif
