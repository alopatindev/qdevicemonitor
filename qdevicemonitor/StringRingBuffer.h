#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QPointer>
#include <QString>
#include <QVector>

class StringRingBuffer : public QObject
{
    Q_OBJECT

    QVector<QString> m_data;
    size_t m_begin;
    size_t m_size;

    StringRingBuffer() {}
    void operator=(StringRingBuffer&) {}

public:
    StringRingBuffer(const size_t capacity)
        : m_data(capacity)
        , m_begin(0)
        , m_size(0)
    {
    }

    ~StringRingBuffer()
    {
    }

    void push(const QString& text)
    {
        if (m_size < static_cast<size_t>(m_data.capacity()))
        {
            ++m_size;
            m_data[m_size - 1] = text;
        }
        else
        {
            m_data[(m_begin + m_size) % m_size] = text;
            ++m_begin;
        }
    }

    class ConstIterator;

    ConstIterator constBegin()
    {
        return ConstIterator(QPointer<StringRingBuffer>(this));
    }

    size_t getCapacity() const
    {
        return m_data.size();
    }

    class ConstIterator
    {
        QPointer<StringRingBuffer> m_buffer;
        size_t m_index;

    public:
        explicit ConstIterator(QPointer<StringRingBuffer> buffer)
            : m_buffer(buffer)
            , m_index(0)
        {
        }

        void operator=(const ConstIterator& other)
        {
            m_buffer = other.m_buffer;
            m_index = other.m_index;
        }

        ConstIterator& operator++(int)
        {
            ++m_index;
            return *this;
        }

        const QString& operator*()
        {
            const size_t currentIndex = (m_buffer->m_begin + m_index) % m_buffer->m_size;
            return m_buffer->m_data[currentIndex];
        }

        bool hasNext() const
        {
            return (m_index + 1) < m_buffer->m_size;
        }

        bool isValid() const
        {
            return m_index < m_buffer->m_size;
        }
    };
};

#endif // RINGBUFFER_H
