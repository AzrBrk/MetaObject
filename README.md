# MetaObject

纯粹的模板元编程库

## meta_stream

```cpp
#include"meta_object_traits.hpp"
#include<iostream>

using namespace meta_typelist;//该命名空间定义元编程流
using namespace list_common_object;//包含一些通用的元输入， 输出流
using namespace collector;//元收集流
```

一个完整meta_stream需要包括其输入流, 输出流. 输入流读取一个类型列表, 并向输出流所绑定的类型列表输出.
输入流,输出流都是元对象, 任意元对象都可以成为输出流，他们拥有相同的调用接口
但是需要注意的输入流必须是拥有返回值的元对象, 即 meta_ret_object

```cpp
 using l = meta_iota<9>;

 //简单输入流
 using miss = meta_istream<l>;

 //简单输出流
 //selectable_list作为容器,该容器不一定是空容器
 using v = meta_ostream<selectable_list<>>;

 //定义元流
 using mstream =
     meta_stream<
     v,//to
     miss//from
     >;

 using ms1 = mstream::update;
 using ms2 = ms1::update;
 using ms3 = ms2::update;

```
```cpp
mstream:
输入流中的列表 = struct exp_repeat::meta_array<0,1,2,3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<0>


ms1:
输入流中的列表 = struct exp_repeat::meta_array<1,2,3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<0>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<1>


ms2:
输入流中的列表 = struct exp_repeat::meta_array<2,3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<0,1>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<2>
```


ms3:
输入流中的列表 = struct exp_repeat::meta_array<3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<0,1,2>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<3>

### 其他输出流，例：带有过滤器的输出流

```cpp
   using m_even_filter_os = meta_appendable_filter_o<selectable_list<>, even_filter>;
   using m_filter_stream = meta_stream<m_even_filter_os, miss>;

```

当该元流传输时，实际上在发生以下情况:

```cpp
 using mfs1 = m_filter_stream::update;
 using mfs2 = mfs1::update;
 using mfs3 = mfs2::update;//...
```

使用 while_constexpr 追踪转移的过程:

```cpp
while_constexpr<
    meta_all_transfer_o<m_even_filter_os, miss>
>{}.recursively_invoke([]<typename mso_t>{
    print_ms<mso_t>("mso status");
});
```

```cpp
mso status:
输入流中的列表 = struct exp_repeat::meta_array<1,2,3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<1>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<2,3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<2>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<3>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1,3>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<4>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1,3>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<5>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1,3,5>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<6>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<1,3,5>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<7>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<8,9>
输出流中的列表 = struct exp_repeat::meta_array<1,3,5,7>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<8>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<9>
输出流中的列表 = struct exp_repeat::meta_array<1,3,5,7>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<9>


mso status:
输入流中的列表 = struct exp_repeat::meta_array<>
输出流中的列表 = struct exp_repeat::meta_array<1,3,5,7,9>
元流的缓存,下一个要操作的类型 = struct no_exist_type

```

## 元流对象 meta_stream_o

用以支持 meta_object中的 meta_looper循环
元流对象是一个定时器元对象 meta_timer_object

一个简单的mso

```cpp
 using simple_mo =
     meta_stream_o<
     3,//update 的次数
     v,//输出流
     miss//输入流
     >;
```

mso 传输

```cpp
using mso_final_t = meta_stream_transfer<simple_mo>;//最终的类型，即meta_stream::to::type
using mso_mo = meta_stream_transfer_mo<simple_mo>;//传输后的元流
using mso_timer = meta_stream_transfer_timer<simple_mo>;//传输之后的定时器对象

print_ms<mso_mo>("mso_mo");
```

```cpp
mso_mo:
输入流中的列表 = struct exp_repeat::meta_array<3,4,5,6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<0,1,2>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<3>
```

重置元流对象的计数器，但元流中的状态不会改变

```
using mso2_mo = meta_stream_transfer_mo<mso_timer::reset<3>>;
print_ms<mso2_mo>("mso2_mo");
```

```cpp
mso2_mo:
输入流中的列表 = struct exp_repeat::meta_array<6,7,8,9>
输出流中的列表 = struct exp_repeat::meta_array<0,1,2,3,4,5>
元流的缓存,下一个要操作的类型 = struct exp_repeat::Idx<6>
```

快捷创建全部转移的流

```cpp
 using all_filter_mo = meta_all_transfer_o<
     m_even_filter_os,//带有过滤器的输出流
     miss//元输入流
 >;

 using filtered_mo = meta_stream_transfer_mo<all_filter_mo>;
 print_ms<filtered_mo>("filtered_mo");
```

相当于

```cpp
using all_filter_mo = meta_stream_o<exp_size<typename miss::type>, m_even_filter_os, miss>;
```

### 特殊的元流对象,循环收集者

```cpp
 using my_loop = make_loop<
     meta_value_limiter_i_co<9>,
     meta_idx_inc_gen_o<3>
 >;

 using collected_mo = meta_collect_ostream<my_loop>;

 std::cout << "\n以下代码展示collector的收集过程:\n";
 constexpr while_constexpr<collected_mo> w_{};
 w_.recursively_invoke(
     []<typename idx_t>{
     std::cout << typeid(meta_to_array<stream_final_t<idx_t>>).name() << '\n';
 }
 );

 std::cout << "\n使用collector::collect可以直接得到最终类型，创建了由3~9的元数组\n";
 type_str<meta_to_array<collector::collect<my_loop>>>();
```

```cpp
以下代码展示collector的收集过程:
struct exp_repeat::meta_array<3>
struct exp_repeat::meta_array<3,4>
struct exp_repeat::meta_array<3,4,5>
struct exp_repeat::meta_array<3,4,5,6>
struct exp_repeat::meta_array<3,4,5,6,7>
struct exp_repeat::meta_array<3,4,5,6,7,8>
struct exp_repeat::meta_array<3,4,5,6,7,8,9>

使用collector::collect可以直接得到最终类型，创建了由3~9的元数组
struct exp_repeat::meta_array<3,4,5,6,7,8,9>
```

需要注意的是：

- 收集者会收集每次循环产生的结果，并将这一序列的结果变成一个输入流
- 收集者不仅可以收集元循环，也可以收集元流传输的结果
- 需要注意的是，如果你不是有意要这么做，被收集的元流中请不要使用meta_ostream做输出流
- 因为每次传输的结果都会被收集，结果将是一系列的meta_ostream
- 若只想收集每次产生的结果，应使用 meta_replace_o 来作为输出流

## 应用，控制可变参函数的参数

```cpp
std::string str{};
int cnt{ 0 };
auto plus_in_pair = [&](auto &&...args)
    {
        std::tuple tp{ std::forward<decltype(args)>(args)... };

        static_assert(!(sizeof...(args) % 2), "error, must not have even args counts");

        using stride_iss = meta_stride_istream<2, meta_iota<sizeof...(args) - 1>>;

        using pair_indices = collect<meta_all_transfer_o<meta_replace_o, stride_iss>>;

        template_func_execute_launcher(pair_indices{}, 
            [&]<typename idx_1, typename idx_2>(exp_list<idx_1, idx_2>, auto& rtp) {
            constexpr std::size_t I1 = idx_1::value;
            constexpr std::size_t I2 = idx_2::value;
            if constexpr (I1 == 0) {
                str = str  + std::get<I1>(rtp);
            }
            else {
                str = str + " + " + std::get<I1>(rtp);
            }
            cnt += std::get<I2>(rtp);
        }, tp);
    };

plus_in_pair("A", 1, "B", 2, "C", 3, "D", 4, "E", 5);

std::cout << str << " = " << cnt << std::endl;
```

```cpp
A + B + C + D + E = 15
```

  
