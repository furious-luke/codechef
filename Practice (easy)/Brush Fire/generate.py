if __name__ == '__main__':
    with open('large.txt', 'w') as out:
        out.write('40\n\n')
        for ii in xrange(40):
            out.write('9999 1 1\n')
            out.write('2 2 3\n')
            pos = 4
            for jj in xrange(2, 5000):
                out.write('3 %d %d %d\n'%(pos, pos + 1, jj/2))
                pos += 2
            for jj in xrange(5000, 10000):
                out.write('1 %d\n'%(jj/2))
            out.write('5003\n\n')
