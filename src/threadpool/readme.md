# ���ܸ���

ʵ����һ���򵥵��̳߳أ�ͨ�������̲߳�����޶ȵؼ����̴߳��������ٵĿ��������Ч�ʡ�<br>

�����Ǹ��̳߳�ʵ�ֵ���Ҫ����͹��ܵ�ϸ�֣�

1. **ThreadPool��**�������װ���̳߳صĹ��ܡ�

2. **˽�нṹ��Pool**���ýṹ�屣�����̳߳ص��ڲ�״̬����������ͬ���Ļ�������`mtx`���������߳�Э��������������`cond`����������־��`isClosed `��ָʾ���Ƿ�رգ��Լ��߳�Ҫִ�е�������У�`tasks`����

3. **���캯��`explicit ThreadPool(size_t threadCount)`**����ָ���������̣߳�`threadCount`����ʼ���̳߳ء����δ�ṩ��������Ĭ��Ϊ 8���ڹ��캯���У�������ָ���������̲߳�Ϊÿ���̷߳���һ�� lambda ������lambda ����ִ������ִ��ѭ���������������壬�������е��������������ִ�����ǣ��������Ϊ���ҳ�δ�ر���ȴ����߳��Ƿ���ģ�����ζ�����ǽ������������С�

4. **��������`~ThreadPool()`**��ȷ���̳߳ض�������ʱ��ȷ������Դ��������isClosed����־����Ϊ true��ʹ�á�notify_all()��֪ͨ���еȴ��̣߳����ͷŻ����塣

5. **`addTask`����ģ��**�����̳߳����һ�������������ṩ�����񣨿������κοɵ��ö���ת����������У�ʹ�á�notify_one()��֪ͨһ���ȴ��̣߳����ͷŻ����塣

������ԣ����̳߳��������йܷ�ʽ�ύ�����Թ�����߳�ͬʱִ�У��Ӷ�ͨ������Ϊÿ�����񴴽��̵߳Ŀ�����������ܡ�


# ǰ��֪ʶ


## C++��������

1. ��ֵ���á��ƶ����塢����ת��
   - [C++11����ֵ���á��ƶ����壨std::move��������ת����std::forward�����](https://blog.csdn.net/weixin_43798887/article/details/117091176?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-3-117091176-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
   - [C++11��std::move��std::forward���׸���ɶ](https://blog.csdn.net/albertsh/article/details/118886009?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-118886009-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
   - [C++��std::move��std::forwardԭ���׻��ܽ�](https://blog.csdn.net/newchenxf/article/details/117995131?ops_request_misc=%7B%22request_id%22:%22171331790616800197077232%22,%22scm%22:%2220140713.130102334.pc_all.%22%7D&request_id=171331790616800197077232&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-4-117995131-null-null.142%5Ev100%5Econtrol&utm_term=std::forward&spm=1018.2226.3001.4187)
2. ����ǿ��ת��
   - [�������C++������ǿ������ת����ʹ�ó���](https://blog.csdn.net/weixin_43798887/article/details/118424172)
3. ����ָ��
   - [C++11������ָ��shared_ptr��weak_ptrԴ�����](https://blog.csdn.net/weixin_43798887/article/details/116464334)
   - [C++������ָ��auto_ptr��unique_ptrԴ�����](https://blog.csdn.net/weixin_43798887/article/details/118104317)
4. std::function��std::bind
   - [C++11��std::function��std::bind�÷����](https://blog.csdn.net/qq_38410730/article/details/103637778?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565173716800227492494%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171565173716800227492494&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-103637778-null-null.142^v100^control&utm_term=std%3A%3Afunction&spm=1018.2226.3001.4187)
5. assert
   - [assert���Ժ������](https://blog.csdn.net/weixin_61561736/article/details/124886522?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565547816800213026389%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565547816800213026389&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-4-124886522-null-null.142^v100^control&utm_term=assert&spm=1018.2226.3001.4187)
6. explict
   - [C++ explicit�ؼ������](https://blog.csdn.net/guoyunfei123/article/details/89003369?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565195216800215019777%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565195216800215019777&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-89003369-null-null.142^v100^control&utm_term=explicit&spm=1018.2226.3001.4187)


## C++���߳�

1. [��C++���ŵ���ͨ�������� (Mutex) C++11 [ C++���� ]](https://blog.csdn.net/m0_75215937/article/details/135041309?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565531916800213071534%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565531916800213071534&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-2-135041309-null-null.142^v100^control&utm_term=mutex&spm=1018.2226.3001.4187)
2. [��C++���ŵ���ͨ��condition_variable������������C++11 [ C++���� ]](https://blog.csdn.net/m0_75215937/article/details/135074541?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565534916800227491852%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565534916800227491852&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135074541-null-null.142^v100^control&utm_term=condition_variable&spm=1018.2226.3001.4187)
3. [��C++���ŵ���ͨ��Lock_guard��Unique_lock C++11 [ C++���� ]](https://blog.csdn.net/m0_75215937/article/details/135041872?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565552716800182786124%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565552716800182786124&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-135041872-null-null.142^v100^control&utm_term=lock_guard&spm=1018.2226.3001.4187)
4. [C++11 ���̣߳�std::thread�����](https://blog.csdn.net/sjc_0910/article/details/118861539?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565575316800185845874%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565575316800185845874&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-118861539-null-null.142^v100^control&utm_term=std%3A%3Athread&spm=1018.2226.3001.4187)
5. [C++11���߳� unique_lock���](https://blog.csdn.net/u012507022/article/details/85909567?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171565479316800186586537%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171565479316800186586537&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-3-85909567-null-null.142^v100^control&utm_term=unique_lock&spm=1018.2226.3001.4187)