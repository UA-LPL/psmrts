#ifndef BulletTracer_hpp

#include <string>

#include <BulletShapeTracer.hpp>
#include <BulletTracerModel.hpp>
#include <ProductSpecification.hpp>

namespace psmrts {
    /**
     * @brief Bullet tracer mesh process class
     * 
     */
    class BulletTracer {
        public:
         BulletTracer( ) { }
         BulletTracer( const psmrts::BulletShapeTracer &bt_t ) :
                                m_model( bt_t ) { }
         virtual ~BulletTracer() { }

        static inline ProductSpecification product_specifications() {
            char text[] = R"({
                "name": "bullet",
                "product": "tracer",
                "type": "tracer",
                "description": "Reads Bullet ray trace files/results and outputs a PSMRTS bullet tracer object",
                "driver": {
                    "name": "bullet",
                    "type": "system?",
                    "aliases": ["BULLET"]
                }
                "parameters": [{
                }]
            })";
            return (ProductSpecification( "bullet", "tracer", json_utils::parse_json_string( text )));
        }

        protected:
            psmrts::BulletShapeTracer m_model;
    }
}