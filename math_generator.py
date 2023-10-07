# !: Matrix columns are stored as arrays

# Set size of matrices
m1 = (4, 4)
m2 = (4, 4)

for k in range(m1[0]):
    for j in range(m2[1])::
        line = f"out[{j}][{k}] = "
        for i in range(m1[1]):
            if i != 0:
                line += " + "

            line += f"m1[{i}][{k}] * m2[{j}][{i}]"
        line += ";"

        print(line)

    print("")
