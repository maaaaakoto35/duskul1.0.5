 //追加機能「複合代入文」
 //複合代入文で式を評価できるかテスト

proc main()
    var a
    var b
   a = 0
   b = 10

    a += 10
    println(a) //10

    a -= 2
    println(a) //8

    a /= 4
    println(a) //2

    a *= 5
    println(a) //10

    a %= 3
    println(a) //1

    a += b - 1
    println(a) //10  
end