/**
 * @file RingBuffer.h
 *
 * Declaration of class RingBuffer
 *
 * @author Max Risler
 *
 * ===> from B-Human
 */

#ifndef __RingBuffer_h_
#define __RingBuffer_h_

#define DEFAULT_RINGBUFFER_SIZE 30

/**
 * @class RingBuffer
 *
 * template class for cyclic buffering of the last n values of Type V
 */
template<class C, int n> 
class RingBuffer
{
    public:
        /** Constructor */
        RingBuffer()
        {
            init();
        }

        /**
         * initializes the RingBufferWithSum
         */
        void init()
        {
            current = n - 1;
            numberOfEntries = 0;
            sum = C();
        }

        /**
         * adds an entry to the buffer
         * \param value value to be added
         */
        void add(C value)
        {
            if(numberOfEntries == n)
                sum -= getEntry(numberOfEntries - 1);
            sum += value;
            current++;
            if(current == n)
                current = 0;
            if(++numberOfEntries >= n)
                numberOfEntries = n;
            buffer[current] = value;
        }

        /**
         * returns an entry
         * \param i index of entry counting from last added (last=0,...)
         * \return a reference to the buffer entry
         */
        C getEntry(int i)
        {
            int j = current - i;
            j %= n;
            if(j < 0)
                j += n;
            return buffer[j];
        }

        C getSum()
        {
            return sum;
        }

        C getMinimum() const
        {
            // Return 0 if buffer is empty
            if(0 == numberOfEntries)
                return C();

            C min = buffer[0];
            for(int i = 0; i < numberOfEntries; i++)
            {
                if(buffer[i] < min)
                    min = buffer[i];
            }
            return min;
        }

        C getMaximum() const
        {
            // Return 0 if buffer is empty
            if(0 == numberOfEntries)
                return C();

            C max = buffer[0];
            for(int i = 0; i < numberOfEntries; i++)
            {
                if(buffer[i] > max)
                    max = buffer[i];
            }
            return max;
        }

        /**
         * returns the average value of all entries
         * \return the average value
         */
        C getAverage() const
        {
            // Return 0 if buffer is empty
            if(0 == numberOfEntries)
                return C();

            return (sum / numberOfEntries);
        }

        /**
         * returns the reciprocal of the average value of all entries
         * \return reciprocal of the average value
         */
        C getReciprocal() const
        {
            // Return 0 if buffer is empty
            if(0 == numberOfEntries)
                return C();

            return 1.0 / (double)(sum / numberOfEntries);
        }

        /**
         * returns an entry
         * \param i index of entry counting from last added (last=0,...)
         * \return a reference to the buffer entry
         */
        C operator[](int i)
        {
            return getEntry(i);
        }

        /**
         * returns a constant entry.
         * \param i index of entry counting from last added (last=0,...)
         * \return a reference to the buffer entry
         */
        C operator[](int i) const
        {
            return buffer[i > current ? n + current - i : current - i];
        }

        inline int getNumberOfEntries() const
        {
            return numberOfEntries;
        }

        /**
         * Returns the maximum entry count.
         * \return The maximum entry count.
         */
        inline int getMaxEntries() const
        {
            return n;
        }

        inline const C* data() const {
            return buffer;
        }

        inline const int offset() const {
            int o = current - 1;
            if (o < 0) {
                o = numberOfEntries - 1;
            }
            return o;
        }

        inline void clear() {
            init();
        }

    private:
        int current;
        int numberOfEntries;
        C buffer[n];
        C sum;
};


template<class C, int n>
class RingBuffer2 {
public:
    RingBuffer<C, n> t;
    RingBuffer<C, n> x;

    inline void clear() {
        t.clear();
        x.clear();
    }

    inline void push_back(const C& t_point, const C& x_point) {
        t.add(t_point);
        x.add(x_point);
    }

    inline int size() const
    {
        return t.getNumberOfEntries();
    }
};

#endif // __RingBuffer_h_
