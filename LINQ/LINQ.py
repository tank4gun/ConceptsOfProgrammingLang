class Sequence:
    def __init__(self, generator):
        self.seq = generator

    def Select(self, function):
        my_seq = self.seq()
        def select_gener():
            for element in my_seq:
                yield function(element)
        self.seq = select_gener
        return self

    def Flatten(self):
        new_seq = []
        my_seq = self.seq()
        for seq in my_seq:
            for element in seq:
                new_seq.append(element)

        def build_new_seq():
            for element in new_seq:
                yield element

        self.seq = build_new_seq
        return self

    def Where(self, function):
        my_seq = self.seq()
        def where_gener():
            for element in my_seq:
                if function(element):
                    yield element
        self.seq = where_gener
        return self

    def Take(self, k):
        my_seq = self.seq()
        def take_gener():
            for i in range(k):
                yield next(my_seq)
        self.seq = take_gener
        return self

    def GroupBy(self, function):
        ans = {}
        my_seq = self.seq()
        for element in my_seq:
            val = function(element)
            if ans.get(val) is None:
                ans[val] = [element]
            else:
                ans[val].append(element)

        def groupby_gener():
            for element in ans:
                yield (element, ans[element])

        self.seq = groupby_gener
        return self

    def OrderBy(self, function):
        my_seq = self.seq()
        sorted_seq = []
        for element in my_seq:
            sorted_seq.append(element)
        sorted_seq = sorted(sorted_seq, key=function)
        def orderby_gener():
            for element in sorted_seq:
                yield element
        self.seq = orderby_gener
        return self

    def ToList(self):
        new_list = []
        my_seq = self.seq()
        for element in my_seq:
            new_list.append(element)
        return new_list

# First task with Fibonacci numbers

def FibGener():
    a = 1
    b = 0
    while True:
        yield a
        a, b = b + a, a


a = Sequence(FibGener)
print(a.Where(lambda x: x % 3 == 0).Select(lambda x: x ** 2 if x % 2 == 0 else x).Take(5).ToList())

# Second task with text

def ReadString(file_name):
    file = open(file_name, "r")
    def read_string():
        for string in file:
            yield string
    return read_string

b = Sequence(ReadString("input.txt"))
print(b.Select(lambda x: x.split()).Flatten().GroupBy(lambda x: x).Select(lambda x: (x[0], len(x[1]))).OrderBy(lambda x: x[1]).ToList())
