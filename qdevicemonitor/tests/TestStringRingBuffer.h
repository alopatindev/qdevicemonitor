#ifndef TESTRINGBUFFER_h
#define TESTRINGBUFFER_h

#include <QtTest/QtTest>
#include <QObject>
#include <QString>
#include "../StringRingBuffer.h"

class TestStringRingBuffer : public QObject
{
    Q_OBJECT

private slots:
    void testCapacity()
    {
        const size_t n = 3;
        StringRingBuffer buf(n);
        QCOMPARE(buf.getCapacity(), n);
    }

    void testIterator()
    {
        StringRingBuffer buf(3);
        buf.push("a");
        buf.push("b");

        auto it = buf.constBegin();
        QCOMPARE(it.isValid(), true);
        QCOMPARE(*it, QString("a"));
        QCOMPARE(it.hasNext(), true);
        it++;
        QCOMPARE(it.isValid(), true);
        QCOMPARE(*it, QString("b"));
        QCOMPARE(it.hasNext(), false);
        it++;
        QCOMPARE(it.isValid(), false);

        buf.push("c");
        buf.push("d");
        it = buf.constBegin();
        QCOMPARE(it.isValid(), true);
        QCOMPARE(*it, QString("b"));
        QCOMPARE(it.hasNext(), true);
        it++;
        QCOMPARE(it.isValid(), true);
        QCOMPARE(*it, QString("c"));
        QCOMPARE(it.hasNext(), true);
        it++;
        QCOMPARE(it.isValid(), true);
        QCOMPARE(*it, QString("d"));
        QCOMPARE(it.hasNext(), false);
        it++;
        QCOMPARE(it.isValid(), false);
    }
};

#endif
