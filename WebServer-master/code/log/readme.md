# ���ܸ���

**BlockQueue Class**<br>
BlockQueue����һ���̰߳�ȫ���������У����Դ洢�κ����͵�Ԫ�ء���ʹ��˫�˶������洢Ԫ�أ�ʹ�û�������ȷ���̰߳�ȫ����ʹ���������������������ߺ�������֮���ͬ����<br>

��Ҫ���ܣ�
- ���캯�������������������������ʼ�����в�������Դ��
- clear()����ն��С�
- empty()���������Ƿ�Ϊ�ա�
- full()���������Ƿ�������
- close()���رն��в�֪ͨ���еȴ��̡߳�
- size()�����ض��еĵ�ǰ��С��
- capacity()�����ض��е����������
- front() & back()�����ʶ��е�ǰ���ͺ�Ԫ�ء�
- Push_back() & Push_front()����Ԫ����ӵ����еĺ����ǰ�档
- pop()���Ӷ���ǰ��ɾ��һ��Ԫ�أ�����ѡ��ʱ��
- flush()��֪ͨһ�����ڵȴ��������ߡ�


**Log Class**<br>
Log��ͨ��ʹ�� BlockQueue ֧���첽��־��¼��������־��¼�����������ں�����������־�ļ��ֻ������������ò�ͬ����־����<br>

��Ҫ���ܣ�
- init()����ʼ����־ϵͳ��
- write()��д����־��Ŀ��֧�ֿɱ������
- flush()������־������ˢ�µ��ļ���
- getLevel() & setLevel()����ȡ�����õ�ǰ��־����
- isOpen()�������־ϵͳ�Ƿ�򿪡�
- appendLogLevelTitle()������־��Ŀ�����־����ǰ׺��
- asyncWrite()������־��Ŀ�Ӷ���д���ļ���
- instance()������ʵ����������
- flushLogThread()�������첽��־��¼���̺߳�����


# ǰ��֪ʶ

1. ���˼·���Բο��� **Linux���̷߳���˱�̣�ʹ��muduoC++�����(��˶)** �еĵ����£����鱾�����Ϳ����������ˡ�
2. #define�� "\\" �����á�
3. '# ## __VA_ARGS__ ##__VA_ARGS__'<br>
   1. [# ## __VA_ARGS__ ##__VA_ARGS__��ʹ��˵��](https://blog.csdn.net/hanxv_1987/article/details/106555870?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171679283516800178567352%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171679283516800178567352&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-1-106555870-null-null.142^v100^control&utm_term=%23define%E9%87%8C%E9%9D%A2%E7%9A%84%23%23__VA__ARGS__&spm=1018.2226.3001.4187)
   2. [#��##��__VA_ARGS__��ʹ��](https://blog.csdn.net/auccy/article/details/88833659?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171679284516800225510710%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=171679284516800225510710&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~hot_rank-1-88833659-null-null.142^v100^control&utm_term=%23define%E9%87%8C%E9%9D%A2%E7%9A%84%23%23__VA__ARGS__&spm=1018.2226.3001.4187)
