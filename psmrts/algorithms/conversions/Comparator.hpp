#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/algorithms/conversions/ConversionTraits.hpp>
#include <psmrts/algorithms/conversions/Extractor.hpp>

namespace psmrts::algorithms::conversions {


/**
   * @brief Extract data from option-like data containers
   * 
   * This class utilizes variant visitor structures to extract data from PSMRTS
   * option data containers. These data containers maintain information
   * regarding PSRMTS products such as shapes and tracers. Product
   * configurations are maintained for each product. Users provide product
   * options that are used to compare with existing instances that have
   * particular paramerizations. The instance parameterizations are compared
   * with product configurations for reuse purposes to minimize data
   * requirements.
   * 
   * This class supports comparisons of any of the supported types in
   * ProductOptions. String types are perhaps the best general comparisons as
   * ConversionsTraits structs allows users to customize output data conversion
   * of strings from particularly double precision data. This can be used to
   * compare instances of ProductOptions that can differ at micrometer scales
   * (see ConversionTraits). However, the same comparison can be made with
   * direct double precision vistors and comparator combinatons, e.g.,
   * Comparator<ProductOption,DoublesVisitor>. 
   * 
   * @author 2026-01-08 Kris J Becker
   */
  template <typename Container, typename Visitor>
    class Comparator {
      public:
        using Type             =  typename Visitor::Type;
        using TypeVector       =  typename Visitor::TypeVector;
        using VisitorExtractor = Extractor<Container, Visitor>;

        Comparator() : m_traits( ConversionTraits() ), 
                       m_default( Visitor::TypeDefault ) { }
        Comparator( const Container &option ) {
          m_option  = option;
          m_traits  = ConversionTraits();
          m_default = Visitor::TypeDefault;
        }      
        Comparator( const Container &option,
                    const ConversionTraits &traits,
                    const Type default_v = Visitor::TypeDefault ) {
          m_option  = option;
          m_traits  = traits;
          m_default = default_v;
        }
        Comparator( const Container &option,
                    const Type default_v,
                    const ConversionTraits &traits = ConversionTraits() ) {
          m_option  = option;
          m_traits  = traits;
          m_default = default_v;
        }        

        virtual ~Comparator() = default;


        /** Return refernce to the current container */
        inline const Container &container() const {
          return ( m_option );
        }

        /** Return refernce to the current container */
        inline size_t size() const {
          return ( this->container().size() );
        }

        /** Return the current data default value */
        inline const Type default_value() const {
          return ( m_default );
        }

        /** Get reference to traits data */
        inline const ConversionTraits &traits() const {
          return ( m_traits );
        }

        /** Equal operator to compare this container of another */
        inline bool operator==( const Container &other ) const {
          return ( compare( other) );
        }
        
        /** Compare another container using the Visitor */
        inline bool compare( const Container &other ) const {
          return ( this->compare( other, this->default_value() ) );
        }

        /**
         * @brief Compare two products with potential differing defaults
         * 
         * This function computes the difference of two double options where
         * differing default values detect extended arrays resulting in a failing
         * comparison. 
         * 
         * Options that do not have all the same number of values will fail by 
         * definition.
         * 
         * @param option1   First option to convert to doubles for comparison
         * @param option2   Second option to convert to doubles for comparison
         * @param default1  Default for first option for failures and missing data
         * @param default2  Default for second option for failures and missing data
         * @param parms     Optional access parameters and traits for comparisons
         *                    Default is to extract all values from each option
         * @return true     If all corresponding values compare within tolerance
         * @return false    If option sizes differ or are not within tolerance
         */
        inline bool compare( const Container &other, const Type default_o ) const {
                         
          size_t this_size  = this->size();
          size_t other_size = other.size();
          size_t max_vals = std::max( this_size, other_size ) ;

          TypeVector this_data, other_data;
          this_data.reserve( this_size );
          other_data.reserve( other_size );

          Visitor this_visitor  = VisitorExtractor::create_visitor( this_data, this->container(), this->traits(), this->default_value() );
          Visitor other_visitor = VisitorExtractor::create_visitor( other_data, other, this->traits(), default_o);

          this->container().visit( this_visitor );
          other.visit( other_visitor );

          return ( this->isequal( this_visitor, this_data, other_visitor, other_data ) );
        }

        /**
         * @brief Compare Visitor conversions of two products with same parameters
         * 
         * This comparison is convenient for options that contain the same number
         * of values, since the same default is used. 
         * 
         * @param option1   First data container to compare
         * @param option2   Second data container to compare with option1
         * @param default1  Provide default for option1
         * @param default2  Provide default for option1
         * @param traits    Set of conversion traits for both extraction processing 
         * @return true     If both options data compare precisely
         * @return false    If the options do not compare
         */
        static inline bool compare( const Container &option1, 
                                    const Container &option2,
                                    const Type default1 = Visitor::TypeDefault,
                                    const Type default2 = Visitor::TypeDefault,
                                    const ConversionTraits &traits = ConversionTraits( )
                                    )  {

            Comparator c1( option1, traits, default1 );
            // Comparator c2( option1, c1.traits() default2 );

          return ( c1.compare( option2, default2 ) );                                    
        }

        /**
         * @brief Compare two container datasets of presumably same sizes
         * 
         * If the two vectors are not of of the same size, they are deemed
         * unequal. Note that both arrays have differing default/invalid types
         * so ensure that condition is considered. If both any dataset contains
         * an invalid data value, the values are considered unequal.
         * 
         * The first option is used to compare the data values for equality.
         * 
         * 
         * @param visitor1 First dataset vistor
         * @param v1       First dataset
         * @param visitor2 Second dataset visitor
         * @param v2       Second dataset
         * @param matches  An option bool map that maps valid and invalid data
         * @return true    If the all data values are equivalent
         * @return false   if any value is not equivalent
         */
        inline bool isequal( const Visitor &visitor1, const TypeVector &v1,
                             const Visitor &visitor2, const TypeVector &v2,
                             std::vector<bool> *matches = nullptr ) 
                             const {

          /// If they are not the same size, we are done!
          if ( v1.size() != v2.size() ) return ( false );

          // Track data integrity for drop through data tests
          bool all_good = true;

          // Robust comparison of two dataset values
          size_t n = std::min( v1.size(), v2.size() );
          if ( nullptr == matches ) {
            for ( size_t i = 0  ; i < n ; i++ ) {
              if ( visitor1.isvalid( v1[i] ) && visitor2.isvalid( v2[i] ) ) {
                if ( !visitor1.isequal( v1[i], v2[i] ) ) return ( false );
              }
            }
          }
          else {
            matches->reserve( n );
            for ( size_t i = 0  ; i < n ; i++ ) {
              if ( visitor1.isvalid( v1[i] ) && visitor2.isvalid( v2[i] ) ) {
                matches->push_back( !visitor1.isequal( v1[i], v2[i] ) );
                if ( !matches->back() ) all_good = false;
              }
            }            
          }

          return ( all_good  );
       }

      private:
        Container        m_option;
        ConversionTraits m_traits;
        Type             m_default;
  };  

}    // namespace psmrts::algoriths::conversions
