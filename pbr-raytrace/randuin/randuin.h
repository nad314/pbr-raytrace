namespace core {
    class RanduinWrynn: public SIMD {
        private:
            static core::buffer<vec4s> deck;
            static int turn;
        public:
            static void construct(const int& cards);
            static void mulligan();
            static const vec4s topdeck(); 
    };
}
