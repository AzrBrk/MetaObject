#include"meta_object_traits.hpp"
#include"offset_pointer.hpp"
#include<iostream>

using namespace offset_pointer;
using namespace meta_typelist;
using namespace list_common_object;
using namespace collector;


template<typename T> constexpr void type_str()
{
    std::cout << typeid(T).name() << '\n';
}

template<class MS> constexpr void print_ms(const char* pre_str)
{
    std::cout << '\n';
    std::cout << pre_str << ':' << '\n';
    std::cout << "输入流中的列表 = " << typeid(meta_to_array<typename MS::from::type>).name() << std::endl;
    std::cout << "输出流中的列表 = " << typeid(meta_to_array<typename MS::to::type>).name() << std::endl;
    std::cout << "元流的缓存,下一个要操作的类型 = " << typeid(typename MS::cache).name() << std::endl;

    std::cout << '\n';
}

struct even_filter
{
    template<class this_list, class idx_type>
    struct apply
    {
        static constexpr bool value = static_cast<bool>(idx_type::value % 2);
    };
};


int main() {

    //meta_stream示例, 一个完整meta_stream需要包括其输入流, 输出流
    //输入流读取一个类型列表, 并向输出流所绑定的类型列表输出
    //输入流,输出流都是元对象, 输入流必须是拥有返回值的元对象, 即meta_ret_object

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

    print_ms<mstream>("mstream");
    print_ms<ms1>("ms1");
    print_ms<ms2>("ms2");
    print_ms<ms3>("ms3");

    //其他输出流，例：带有过滤器的输出流
    using m_even_filter_os = meta_appendable_filter_o<selectable_list<>, even_filter>;
    using m_filter_stream = meta_stream<m_even_filter_os, miss>;

    std::cout << "当该元流传输时，实际上在发生以下情况:" << std::endl;
    using mfs1 = m_filter_stream::update;
    using mfs2 = mfs1::update;
    using mfs3 = mfs2::update;//...

    print_ms<m_filter_stream>("m_filter_stream");
    print_ms<mfs1>("mfs1");
    print_ms<mfs2>("mfs2");
    print_ms<mfs3>("mfs3");

    std::cout << "现在使用 while_constexpr 追踪转移的过程:" << std::endl;

    while_constexpr<
        meta_all_transfer_o<m_even_filter_os, miss>
    >{}.recursively_invoke([]<typename mso_t>{
        print_ms<mso_t>("mso status");
    });

    //元流对象， meta_stream_o
    //用以支持 meta_object中的 meta_looper循环
    //元流对象是一个定时器元对象 meta_timer_object

    //简单 mso
    using simple_mo =
        meta_stream_o<
        3,//update 的次数
        v,//输出流
        miss//输入流
        >;

    std::cout << "simple_mo" << " 的长度为: " << exp_size<simple_mo> << std::endl;

    //mso 传输
    using mso_final_t = meta_stream_transfer<simple_mo>;//最终的类型，即meta_stream::to::type
    using mso_mo = meta_stream_transfer_mo<simple_mo>;//传输后的元流
    using mso_timer = meta_stream_transfer_timer<simple_mo>;//传输之后的定时器对象

    print_ms<mso_mo>("mso_mo");

    //重置元流对象的计数器，但元流中的状态不改变
    using mso2_mo = meta_stream_transfer_mo<mso_timer::reset<3>>;
    print_ms<mso2_mo>("mso2_mo");

    //快捷创建全部转移的流
    using all_filter_mo = meta_all_transfer_o<
        m_even_filter_os,//带有过滤器的输出流
        miss//元输入流
    >;

    using filtered_mo = meta_stream_transfer_mo<all_filter_mo>;
    print_ms<filtered_mo>("filtered_mo");

    //特殊的元流对象,循环收集者
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

    std::cout << "\n使用collector::collect可以直接得到最终类型，创建了由3~9的元\n";
    type_str<meta_to_array<collector::collect<my_loop>>>();

    //应用，控制可变参函数的参数

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
  }
