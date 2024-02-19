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
    std::cout << "�������е��б� = " << typeid(meta_to_array<typename MS::from::type>).name() << std::endl;
    std::cout << "������е��б� = " << typeid(meta_to_array<typename MS::to::type>).name() << std::endl;
    std::cout << "Ԫ���Ļ���,��һ��Ҫ���������� = " << typeid(typename MS::cache).name() << std::endl;

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

    //meta_streamʾ��, һ������meta_stream��Ҫ������������, �����
    //��������ȡһ�������б�, ������������󶨵������б����
    //������,���������Ԫ����, ������������ӵ�з���ֵ��Ԫ����, ��meta_ret_object

    using l = meta_iota<9>;

    //��������
    using miss = meta_istream<l>;

    //�������
    //selectable_list��Ϊ����,��������һ���ǿ�����
    using v = meta_ostream<selectable_list<>>;

    //����Ԫ��
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

    //������������������й������������
    using m_even_filter_os = meta_appendable_filter_o<selectable_list<>, even_filter>;
    using m_filter_stream = meta_stream<m_even_filter_os, miss>;

    std::cout << "����Ԫ������ʱ��ʵ�����ڷ����������:" << std::endl;
    using mfs1 = m_filter_stream::update;
    using mfs2 = mfs1::update;
    using mfs3 = mfs2::update;//...

    print_ms<m_filter_stream>("m_filter_stream");
    print_ms<mfs1>("mfs1");
    print_ms<mfs2>("mfs2");
    print_ms<mfs3>("mfs3");

    std::cout << "����ʹ�� while_constexpr ׷��ת�ƵĹ���:" << std::endl;

    while_constexpr<
        meta_all_transfer_o<m_even_filter_os, miss>
    >{}.recursively_invoke([]<typename mso_t>{
        print_ms<mso_t>("mso status");
    });

    //Ԫ������ meta_stream_o
    //����֧�� meta_object�е� meta_looperѭ��
    //Ԫ��������һ����ʱ��Ԫ���� meta_timer_object

    //�� mso
    using simple_mo =
        meta_stream_o<
        3,//update �Ĵ���
        v,//�����
        miss//������
        >;

    std::cout << "simple_mo" << " �ĳ���Ϊ: " << exp_size<simple_mo> << std::endl;

    //mso ����
    using mso_final_t = meta_stream_transfer<simple_mo>;//���յ����ͣ���meta_stream::to::type
    using mso_mo = meta_stream_transfer_mo<simple_mo>;//������Ԫ��
    using mso_timer = meta_stream_transfer_timer<simple_mo>;//����֮��Ķ�ʱ������

    print_ms<mso_mo>("mso_mo");

    //����Ԫ������ļ���������Ԫ���е�״̬���ı�
    using mso2_mo = meta_stream_transfer_mo<mso_timer::reset<3>>;
    print_ms<mso2_mo>("mso2_mo");

    //��ݴ���ȫ��ת�Ƶ���
    using all_filter_mo = meta_all_transfer_o<
        m_even_filter_os,//���й������������
        miss//Ԫ������
    >;

    using filtered_mo = meta_stream_transfer_mo<all_filter_mo>;
    print_ms<filtered_mo>("filtered_mo");

    //�����Ԫ������,ѭ���ռ���
    using my_loop = make_loop<
        meta_value_limiter_i_co<9>,
        meta_idx_inc_gen_o<3>
    >;

    using collected_mo = meta_collect_ostream<my_loop>;

    std::cout << "\n���´���չʾcollector���ռ�����:\n";
    constexpr while_constexpr<collected_mo> w_{};
    w_.recursively_invoke(
        []<typename idx_t>{
        std::cout << typeid(meta_to_array<stream_final_t<idx_t>>).name() << '\n';
    }
    );

    std::cout << "\nʹ��collector::collect����ֱ�ӵõ��������ͣ���������3~9��Ԫ\n";
    type_str<meta_to_array<collector::collect<my_loop>>>();

    //Ӧ�ã����ƿɱ�κ����Ĳ���

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
