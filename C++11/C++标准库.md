## nullptr和std::nullptr_t
nullptr用来代替NULL或0,它可以转换为任何指针,但是不会转化为任何整数类型

std::nullptr_t定义在cstddef中,是一个新类型,和int,char地位差不多

## Uniform Initialization和Initializer List
* C++引入了"一致性初始化",面对任何初始化动作,你都可以使用大括号
* 窄化对于大括号而言不成立
* std::initializer_list<>,用来支持一系列值进行初始化
```
void print(std::initializer_list<int> vals){
    for(auto p=vals.begin();p!vals.end();++p){
        std::cout<<*p<<std::endl;
    }
}

print({12,3,4,5,6});
//所以initializer_list<>的实参就是{...}
```

*除此之外,explict和initialization list碰撞也会发生一些有趣的事情

## Range-Based for循环

## Move语义和Rvalue Reference
* Move semantic用以避免非必要拷贝和临时对象

func(x+x);//moves contents of temporary rvalue

func(std::move(x));//

## 新式的字符串字面常量
