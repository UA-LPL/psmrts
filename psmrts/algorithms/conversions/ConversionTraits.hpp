#ifndef ConversionTraits_hpp
#define ConversionTraits_hpp

#include <algorithm>
#include <functional>

#include <psmrts/core/PsmrtsUtilities.hpp>

namespace psmrts::algorithms::conversions {

/**
 * @brief Conversion traits to control formating and precision
 * 
 * This class maintains defaults and allows user control of conversion
 * utilities that formulate access to the ProductOption object. This provides
 * consistent conversions to represent and test data of differing types. 
 * 
 * @author 2026-01-08 Kris J Becker
 */
  class ConversionTraits {
    public:
      inline static const size_t DigitsPrecision = 9;
      inline static const double DoubleTolerance = 1.0e-9;
      inline static const int    JsonSpacing = -1;

      /** Default constructor */
      ConversionTraits(  ) {
        digits_precision = DigitsPrecision;
        double_tolerance = DoubleTolerance;
        json_space       = JsonSpacing;
      }
      
      /** Specify digits and optional parameters */
      ConversionTraits(const size_t digits,
                        const double tolerance = DoubleTolerance,
                        const int    spacing  = JsonSpacing ) {
        digits_precision = digits;
        double_tolerance = tolerance;
        json_space       = spacing;
      }
      ConversionTraits( const double tolerance,
                        const size_t digits  = DigitsPrecision,
                        const int    spacing = JsonSpacing ) {
        digits_precision = digits;
        double_tolerance = tolerance;
        json_space       = spacing;
      }     

      virtual ~ConversionTraits() = default;

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

    private:
      size_t digits_precision;
      double double_tolerance;
      int json_space;
  };


  /**
   * @brief Class maintains access control for extraction of ProductOption data
   * 
   * Users can specify the control of which values to extract from ProductOption
   * instances and how to establish default values representing conversion
   * failures. This allows easier methods for testing for and comparing data in
   * the PSMRTS system.
   * 
   * This class provides a safe and consistent process to access data scalar and
   * array data sets using the ProductOption visitors. It is critically
   * important that users of this class provide exact sizes of the data size
   * being processed or invalid buffer indexing can occur. 
   * 
   * The method extractor() provides a convenience helper function for
   * determining which index into the dataset array is required to process the
   * extraction request. The "index_t" parameter specifies the starting 0-based
   * index into the array. The "count_t" parameter is the total number of
   * dataset values to extract. For any given extraction, there will always be
   * count() results in the extracted dataset. Indexes that exceed dataset array
   * sizes are intended to result in the default value define in the Vistor
   * functor.
   * 
   * See the extractor() method for details.
   * 
   * @author 2026-01-08 Kris J Becker
   */
  class ConversionParameters {
    public:
    /** Define an extractor function type for extractions */
      using ExtractorFunction = std::function<void(const bool addit, const size_t index)>;

      /** Default constructor for a no values */
      ConversionParameters() { 
        index_t        = 0;
        count_t        = 0;
        traits_t = ConversionTraits( );
      }
      /** Get a single value from an array */
      ConversionParameters(const size_t ith, 
                           const ConversionTraits &t = ConversionTraits() ) {
        index_t        = ith;
        count_t        = 1;
        traits_t       = t;
      }    
      /** Get a range of values from an array */
      ConversionParameters(const size_t ith, const size_t n,
                           const ConversionTraits &t = ConversionTraits() ) {
        index_t        = ith;
        count_t        = n;
        traits_t       = t;
      }
      virtual ~ConversionParameters() = default;


      /** Returns 0-based index into array - 0 for scalar */
      inline size_t index() const {
        return ( index_t );
      }

      /** Number of values to extract - can expand scalars */
      inline size_t count() const {
        return ( count_t );
      }
      
      /** Returns extractiom traits */
      inline const ConversionTraits &traits() const {
        return ( traits_t  );
      }

      /**
       * @brief Determine if the dataset is completely processed
       * 
       * This method should dictate the duration of the loop run for every 
       * extraction to ensure the dataset is processed properly. The dataset
       * visitor must use this method to determine if dataset processing is to
       * continue at the given index. It will continue will this method returns
       * true. The add_valid_value() and dataset_index() methods are required to
       * be used so to correctly process. 
       * 
       * @see extractor().
       * 
       * @param index_d     0-based index into the dataset array. 
       * @param max_count_d Total values in the dataset being processed
       * @return true       If processing at the current loop index should occur
       * @return false      If the processing loop of the dataset terminates at
       *                     this index.
       */
      inline bool done( const size_t index_d, 
                        const size_t max_count_d ) const {
        if ( index_d >= this->count() ) return ( true );
        return ( false );
      }

      /**
       * @brief Determine if the index is valid given traits and array size
       * 
       * This function is used to determine if the given 0-based index indicates
       * it should be added to the extracted data set. The index() in the
       * ConversionParameters struct is the starting index of the extraction
       * array. The max_count parameter indicates the absolute size of the data 
       * set, where 1 indicates a scalar data set. The count() is the requested
       * size which could exceed the size of the dataset.
       * 
       * Only when this returns true should a dataset value be added to the
       * output extraction result. The actual index into the dataset must be
       * computed by the dataset_index() method. Also the done() method must be
       * called first to determine if the loop should be terminated and the
       * requested extraction is compete. If done() returns true, the extraction
       * is complete and no other data should be processed.
       * 
       * @see extractor()
       * 
       * @param index_d   0-based index into dataset
       * @param max_count Size of the dataset where 1 indicates as scalar
       * @return true     If the data set value should be extracted
       * @return false    If the default value should be appended
       */
      inline bool add_valid_value(const size_t index_d, 
                                  const size_t max_count_d ) const {
        size_t ith = index_d + this->index();
        if ( ith     >= max_count_d )   return ( false );
        if ( index_d >= this->count() ) return ( false );
        return ( true );
      }
      
      /**
       * @brief Compute dataset index based upon parameterized config
       * 
       * This method should be used in conjunction with the done() method and
       * add_valid_value() methods of this struct to determine which data gets
       * extracted. See the done() method for details.
       * 
       * @see extractor().
       * 
       * @param index_d     0-based dataset index currently being processed
       * @param max_count_d Total number of dataset values
       * @return            The index into the dataset. It will always
       *                      return a valid index to prevent array overrun.
       */
      inline size_t dataset_index( const size_t index_d, 
                                   const size_t max_count_d) const {
        size_t ith = index_d + this->index();
        if ( ith >= max_count_d ) {
          return ( ( max_count_d == 0 ) ? 0 : max_count_d - 1 );
        }
        return ( ith );
      }


      /**
       * @brief Extraction method to apply to a dataset using a lambda/function
       * 
       * This method provides a extraction processor using the configuration of 
       * the parameters contain herein. This is intended to simplify Visitor
       * functors that need to iterate safely and consistently for all data
       * types.
       * 
       * Visitor datatype extractor functions (defined above) require the
       * maximum size of the dataset being extracted (1 for scalars, size() for
       * arrays) and a function that accepts a bool and size_t index into its
       * data set array. NOTE users of this extractor method must provide the
       * correct/exact size of the data array being processed. However, not when
       * the "addit" parameter is false, the index cannot be guaranteed to be
       * valid and the result is typically the last index in the array/scalar.
       * The only exception when that datasize is 0, which in that case, the
       * default value should always result within the process() function.
       * 
       * An example of how to use this for a scalar value:
       * 
       * @code
       *  inline void operator()( const bool b ) {
       *    auto process = [&]( const bool addit, const size_t index ) {
       *      if ( addit ) {
       *       m_datum.push_back( ( b ? "true" : "false" ) );
       *      }
       *      else {
       *        m_datum.push_back ( default_value() );
       *      }
       *    };
       *    // Note scalars always has a size of 1
       *    parameters().extractor( 1, process );
       *  }
       * @endcode
       * 
       * For arrays, the implementation is similar but differs in size:
       * 
       * @code
       *  inline void operator()( const std::vector<std::string> &s_array ) {
       *    auto process = [&]( const bool addit, const size_t index ) {
       *      if ( addit ) {
       *        m_datum.push_back( s_array[index] ); 
       *      }
       *      else {
       *        m_datum.push_back ( default_value() );
       *      }
       *    };
       *    parameters().extractor( s_array.size(), process ); 
       *  }      
       * @endcode
       * 
       * In all cases, when "addit" is false, correct indexing cannot be
       * guarenteed and should result in a default value result.
       * 
       * @param max_dataset_size Size of the dataset array that the extraction
       *                           is processing. Ranges should be 0 to size()-1. 
       * @param processor        Lamda or function that accepts a bool and index
       *                           that will process the extraction request.
       * @return size_t          Total number of calls made to the processor()
       *                           function.
       */
      inline size_t extractor( const size_t max_dataset_size, 
                               ExtractorFunction processor) const {

        size_t n = 0;
        for ( size_t i = 0 ; !done(i, max_dataset_size ) ; i++, n++ ) {
          bool   addit = add_valid_value( i, max_dataset_size );
          size_t index = dataset_index(i, max_dataset_size);
          processor( addit, index );
        } 
        return ( n );      
      }

    private:
      size_t index_t;
      size_t count_t;
      ConversionTraits traits_t;
  };

} // namspace psmrts::algorithms::conversions

#endif
