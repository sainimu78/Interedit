#pragma once
#include "Niflect/Serialization/Format/Json/RapidjsonWrapper.h"
#include "Niflect/Serialization/Tree/TreeReadWrite2.h"
#include "Niflect/Base/NiflectBase.h"

//todo: �ɴ�������ͬ����, Ϊ�������������Ӻ�׺2, δ����ɾ���ɴ��벢�Ƴ���׺2
//todo: JsonTreeReadWriteʵ�ִ�������ȱ��:
//	1. ���������, ���ֻ�ȡ���������е�ֵ, ��ֱ��ͨ��json�ڵ��ǿɻ�ȡ��
//	2. δʵ����ȷ����create��add�Ĳ���
//	3. δʵ����ȷ��ɾ������
//	4. ����2��3�����޷��������ı�������, �ȴ����յĽڵ�, �����, ��������Ч�򲻼��뵽���ڵ�
//	5. ����ͬ��rapidjson����ķ�װ��ʽ, ��������, ��Ҫ����ͬ��ָ��, ��Ҳ������һЩ�߼�����δ��������Ĵ���
//	6. ���л�ֻ��RwNode��ʵ�ַ����CreateRwNode��Ҫ���븸�ڵ���Ϊ����, �����RwNode�Ŀ�ܱ������Ҫ����, ��Ӧ��Ҫʵ�ִ����սڵ�
//	��˼ƻ�ר�Ž��json���л�����, �Ҳ��ų�������д�����ʵ��, ����Ϊ��ͨ�õĿ��:
//	1. ��Stream��Format���ֶ���
//	2. ���ڱ���Ĺ�����, Format�ɹ������ͻ���, ����һЩ���ھ������д��Stream��״̬
//	3. д�Ĺ��������ͽṹ��������, ÿ���ڵ����, ͨ��Format�������д��Stream
//	��һ��˼·�ǽ�RwNodeʵ�ֳɶ��������Ͷ���, ����Ҫ�����������л���, ֻ��Ҫ�Բ�ͬ��ʵ�����ṹ��д����, �������ܿ��ܲ�������, ��RwNode�������������в��ҹ���, ����������л���Ҳ���в��ҹ���
namespace TreeReadWrite2
{
	using CRwJson = Niflect::CJson<Niflect::CMemory>;
	
	class CJsonRwNode;
	class CJsonRwValue;
	class CJsonRwArray;
	class CJsonRwDocument;

	static Niflect::TSharedPtr<CJsonRwNode> CreateJsonRwNode(CJsonRwDocument* rwDoc, CRwJson::Object* raw, bool ownedRaw);
	static Niflect::TSharedPtr<CJsonRwValue> CreateJsonRwValue(CJsonRwDocument* rwDoc, CJsonRwNode* owner);
	static Niflect::TSharedPtr<CJsonRwArray> CreateJsonRwArray(CJsonRwDocument* rwDoc, CJsonRwNode* owner);
	//static void UpdateInternalRawJsonNodeAddr(CJsonRwNode* rwNode);
	static CJsonRwNode* CastJsonRwNode(CRawRwNode* base);
	
	//todo: ��д���̶����������д����, Ӧ�����̶߳ѷ���һС��ר��json���л����ڴ����ڴ��ʹ��
	//	���Ѹ���CDefaultMemory, �ɳ����Ľ�
	class CJsonRwDocument : public CRwDocument
	{
		typedef CRwDocument inherited;
	public:
		CJsonRwDocument(CRwJson::Document& jd, CRwJson::Object& joRoot)
			: inherited(CreateJsonRwNode(this, &joRoot, false))
			, m_jd(jd)
		{
		}
		//~CJsonRwDocument()
		//{
		//	for (auto& it : m_vecTemp)
		//	{
		//		it->~GenericValue();
		//		m_jd.GetAllocator().Free(it);
		//	}
		//}

	public:
		virtual CRwNode CreateNode() override
		{
			using namespace rapidjson;
			auto raw = static_cast<CRwJson::Object*>(m_jd.GetAllocator().Malloc(sizeof(CRwJson::Object)));
			Niflect::GenericInstanceInvokeConstructor<CRwJson::Object>(raw, Type::kObjectType);
			//m_vecTemp.push_back(p);
			return CreateJsonRwNode(this, raw, true);
		}
	public:
		static CJsonRwDocument* Cast(CRwDocument* base)
		{
			ASSERT(dynamic_cast<CJsonRwDocument*>(base) != NULL);
			return static_cast<CJsonRwDocument*>(base);
		}

	public:
		CRwJson::Document& m_jd;
		//TArrayNif<CRwJson::Object*> m_vecTemp;
	};

	class CJsonRwNode : public CRawRwNode
	{
		typedef CRawRwNode inherited;
		typedef Niflect::CString CString;
	public:
		CJsonRwNode(CJsonRwDocument* rwDoc, CRwJson::Object* raw, bool ownedRaw)
			: inherited(rwDoc)
			, m_raw2(raw)
			, m_ownedRaw2(ownedRaw)
		{

		}
		~CJsonRwNode()
		{
			this->DeleteOwnedRaw();
		}

	private:
		void DeleteOwnedRaw()
		{
			if (m_ownedRaw2)
			{
				auto& jd = CJsonRwDocument::Cast(m_rwDoc)->m_jd;
				Niflect::GenericInstanceInvokeDestructor<CRwJson::Object>(m_raw2);
				jd.GetAllocator().Free(m_raw2);
			}
		}

	private:
		CRwNode& CacheRwNode(uint32 idx, CRwJson::Object* jo, const CString& name) const
		{
			//Ϊ������, ����ȱ��, �����߼���֧��ɾ��
			//����ͨ���򵥱���ȫ��item���ж�Ӧ, ����Ϊ����, ��Ϊһ��ֻ�����һ��item, ������Ҫ����ȫ��
			//�����Ҫ֧��ɾ��, ��Ҫ����ʵ�ַ�ʽ, ������ɾ��ʱר�Ŵ�����
			ASSERT(m_vecNameAndRwNode.size() <= this->GetRaw()->MemberCount());
			if (m_vecNameAndRwNode.size() <= idx)
				m_vecNameAndRwNode.resize(idx + 1);
			if (m_vecNameAndRwNode[idx].m_node == NULL)
			{
				auto rwNode = CreateJsonRwNode(CJsonRwDocument::Cast(m_rwDoc), jo, false);
				m_vecNameAndRwNode[idx] = { name, rwNode };
			}
			return m_vecNameAndRwNode[idx].m_node;
		}
		void UpdateCache() const
		{
			this->UpdateCacheFromIndex(0);
		}
		void UpdateCacheFromIndex(uint32 fromIndex) const
		{
			auto raw = this->GetRaw();
			auto itBegin = raw->MemberBegin();
			for (uint32 idx0 = fromIndex; idx0 < m_vecNameAndRwNode.size(); ++idx0)
			{
				auto& item = m_vecNameAndRwNode[idx0];
				if (auto rw = static_cast<CJsonRwNode*>(item.m_node.Get()))
				{
					ASSERT(!rw->m_ownedRaw2);
					rw->m_raw2 = &(itBegin + idx0)->value;
					//UpdateInternalRawJsonNodeAddr(rw);
				}
			}
		}

	public:
		virtual CRwNode FindNode(const CString& name) const override
		{
			auto raw = this->GetRaw();
			auto itFound = raw->FindMember(name.c_str());
			if (itFound != raw->MemberEnd())
			{
				auto idx = static_cast<uint32>(std::distance(raw->MemberBegin(), itFound));
				auto& rwNode = this->CacheRwNode(idx, &itFound->value, name);
				return rwNode;
			}
			return NULL;
		}
		virtual CRwNode& AddNode(const CString& name, const CRwNode& rwNode, uint32* insertedIndex) override
		{
			using namespace rapidjson;
			auto casted = CastJsonRwNode(rwNode.Get());
			CRwJson::SizeType idx = INDEX_NONE;
			auto& jd = CJsonRwDocument::Cast(m_rwDoc)->m_jd;
			CRwJson::Object rawName(name.c_str(), jd.GetAllocator());
			auto raw = this->GetRaw();
			int oldCapa = raw->MemberCapacity();
			auto oldCount = raw->MemberCount();
			idx = raw->MemberCount();
			ASSERT(casted->m_ownedRaw2);
			raw->AddMember(rawName, *casted->GetRaw(), jd.GetAllocator());
			auto rawNew = &(raw->MemberBegin() + idx)->value;
			casted->MarkRawTaken(rawNew);
			auto newCount = raw->MemberCount();
			auto newCapa = raw->MemberCapacity();
			this->CacheRwNode(idx, rawNew, name);
			if (oldCapa != newCapa)
				this->UpdateCache();
			if (insertedIndex != NULL)
				*insertedIndex = idx;
			return m_vecNameAndRwNode[idx].m_node;;
		}

	public:
		virtual uint32 GetNodesCount() const override
		{
			auto raw = this->GetRaw();
			return raw->MemberCount();
		}
		virtual CNamedRwNode& GetNodeImpl(uint32 idx) const override
		{
			auto raw = this->GetRaw();
			auto it = raw->MemberBegin() + idx;
			this->CacheRwNode(idx, &it->value, it->name.GetString());
			return m_vecNameAndRwNode[idx];
		}

	public:
		CRwJson::Object* GetRaw() const
		{
			return m_raw2;
		}
		void MarkRawTaken(CRwJson::Object* rawNew)
		{
			this->DeleteOwnedRaw();
			m_ownedRaw2 = false;
			m_raw2 = rawNew;
		}

	public:
		virtual CRwValue& ToValueImpl() override
		{
			if (m_rwValue == NULL)
			{
				//ASSERT((!m_raw->IsObject() || (m_raw->IsArray() && m_raw->Empty())));
				m_rwArray = NULL;
				m_rwValue = CreateJsonRwValue(CJsonRwDocument::Cast(m_rwDoc), this);
			}
			return m_rwValue;
		}
		virtual CRwArray& ToArrayImpl() override
		{
			if (m_rwArray == NULL)
			{
				//ASSERT(m_raw->IsArray() || m_raw->IsNull());
				m_rwValue = NULL;
				m_rwArray = CreateJsonRwArray(CJsonRwDocument::Cast(m_rwDoc), this);
				auto raw = this->GetRaw();
				if (!raw->IsArray())
					raw->SetArray();
			}
			return m_rwArray;
		}

		mutable Niflect::TArrayNif<CNamedRwNode> m_vecNameAndRwNode;
		mutable CRwValue m_rwValue;
		mutable CRwArray m_rwArray;
		CRwJson::Object* m_raw2;
		bool m_ownedRaw2;
	};

	class CJsonRwValue : public CRawRwValue
	{
		typedef CRawRwValue inherited;
		typedef Niflect::CString CString;
	public:
		CJsonRwValue(CJsonRwDocument* rwDoc, CJsonRwNode* owner)
			: inherited(rwDoc)
			, m_owner(owner)
		{
		}

	private:
		CRwJson::Object* GetRaw() const
		{
			return m_owner->GetRaw();
		}

	public:
		virtual bool GetBool(bool defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsBool())
					return raw->GetBool();
			}
			return defaultValue;
		}
		virtual int32 GetInt32(int32 defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsInt())
					return raw->GetInt();
			}
			return defaultValue;
		}
		virtual uint32 GetUint32(uint32 defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsUint())
					return raw->GetUint();
			}
			return defaultValue;
		}
		virtual float GetFloat(float defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsFloat())
					return raw->GetFloat();
			}
			return defaultValue;
		}
		virtual double GetDouble(double defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsDouble())
					return raw->GetDouble();
			}
			return defaultValue;
		}
		virtual CString GetString(const CString& defaultValue) const override
		{
			if (auto raw = this->GetRaw())
			{
				if (raw->IsString())
					return raw->GetString();
			}
			return defaultValue;
		}
		virtual bool GetRawData(CRwRawData& rawData) const override
		{
			ASSERT(false);
			return false;
			//CRwJson::SizeType rawSize;
			//return m_raw->GetRawData(rawData.m_data, rawSize);
			//rawData.m_size = rawSize;
		}

	public:
		virtual void SetBool(bool value) override
		{
			if (auto raw = this->GetRaw())
				*raw = value;
		}
		virtual void SetInt32(int32 value) override
		{
			if (auto raw = this->GetRaw())
				*raw = value;
		}
		virtual void SetUint32(uint32 value) override
		{
			if (auto raw = this->GetRaw())
				*raw = value;
		}
		virtual void SetFloat(float value) override
		{
			if (auto raw = this->GetRaw())
				*raw = value;
		}
		virtual void SetDouble(double value) override
		{
			if (auto raw = this->GetRaw())
				*raw = value;
		}
		virtual void SetString(const CString& value) override
		{
			if (auto raw = this->GetRaw())
			{
				auto& jd = CJsonRwDocument::Cast(m_rwDoc)->m_jd;
				raw->SetString(value.c_str(), jd.GetAllocator());
			}
		}
		virtual void SetRawData(const void* data, size_t size) override
		{
			ASSERT(false);
			//m_raw->SetRawData(data, static_cast<CRwJson::SizeType>(size));
		}

		CJsonRwNode* m_owner;
	};

	class CJsonRwArray : public CRawRwArray
	{
		typedef CRawRwArray inherited;
		typedef Niflect::CString CString;
	public:
		CJsonRwArray(CJsonRwDocument* rwDoc, CJsonRwNode* owner)
			: inherited(rwDoc)
			, m_owner(owner)
		{

		}

	private:
		CRwJson::Object::Array GetRaw() const
		{
			return m_owner->GetRaw()->GetArray();
		}
		CRwNode& CacheRwNode(uint32 idx, CRwJson::Object* jo) const
		{
			if (m_vecRwNode.size() <= idx)
				m_vecRwNode.resize(idx + 1);
			auto& rwNode = m_vecRwNode[idx];
			if (rwNode == NULL)
				rwNode = CreateJsonRwNode(CJsonRwDocument::Cast(m_rwDoc), jo, false);
			return rwNode;
		}
		void UpdateCache() const
		{
			auto ja = this->GetRaw();
			for (uint32 idx0 = 0; idx0 < m_vecRwNode.size(); ++idx0)
			{
				auto& item = m_vecRwNode[idx0];
				if (auto rw = CastJsonRwNode(item.Get()))
				{
					ASSERT(!rw->m_ownedRaw2);
					rw->m_raw2 = &ja[idx0];
					//UpdateInternalRawJsonNodeAddr(rw);
				}
			}
		}

	public:
		virtual uint32 GetItemsCount() const override
		{
			auto ja = this->GetRaw();
			return ja.Size();
		}
		virtual CRwNode& GetItemNodeImpl(uint32 idx) override
		{
			auto ja = this->GetRaw();
			auto& raw = ja[idx];
			auto& rwNode = this->CacheRwNode(idx, &raw);
			return rwNode;
		}

	protected:
		virtual void AddItemNodeImpl(const CRwNode& rwItem) override
		{
			auto ja = this->GetRaw();
			auto idx = ja.Size();
			auto& jd = CJsonRwDocument::Cast(m_rwDoc)->m_jd;
			using namespace rapidjson;
			auto oldCapa = ja.Capacity();
			auto casted = CastJsonRwNode(rwItem.Get());
			ASSERT(casted->m_ownedRaw2);
			ja.PushBack(*casted->GetRaw(), jd.GetAllocator());
			auto& raw = ja[idx];
			auto rawNew = &raw;
			casted->MarkRawTaken(rawNew);
			auto newCapa = ja.Capacity();
			auto& rwNode = this->CacheRwNode(idx, rawNew);
			if(oldCapa != newCapa)
				this->UpdateCache();
		}

	public:
		CJsonRwNode* m_owner;
		mutable Niflect::TArrayNif<CRwNode> m_vecRwNode;
	};

	Niflect::TSharedPtr<CJsonRwNode> CreateJsonRwNode(CJsonRwDocument* rwDoc, CRwJson::Object* raw, bool ownedRaw)
	{
		return Niflect::MakeSharable(Niflect::CInstance::New<CJsonRwNode>(rwDoc, raw, ownedRaw));
	}
	Niflect::TSharedPtr<CJsonRwValue> CreateJsonRwValue(CJsonRwDocument* rwDoc, CJsonRwNode* owner)
	{
		return Niflect::MakeSharable(Niflect::CInstance::New<CJsonRwValue>(rwDoc, owner));
	}
	Niflect::TSharedPtr<CJsonRwArray> CreateJsonRwArray(CJsonRwDocument* rwDoc, CJsonRwNode* owner)
	{
		return Niflect::MakeSharable(Niflect::CInstance::New<CJsonRwArray>(rwDoc, owner));
	}
	//void UpdateInternalRawJsonNodeAddr(CJsonRwNode* rwNode)
	//{
	//	ASSERT(false);
	//	//if (auto rwValue = rwNode->m_rwValue.Get())
	//	//{
	//	//	ASSERT(rwNode->m_rwArray == NULL);
	//	//	auto casted = static_cast<CJsonRwValue*>(rwValue);
	//	//	casted->m_raw = rwNode->m_raw;
	//	//}
	//	//else if (auto rwArray = rwNode->m_rwArray.Get())
	//	//{
	//	//	ASSERT(rwNode->m_rwValue == NULL);
	//	//	auto casted = static_cast<CJsonRwArray*>(rwArray);
	//	//	casted->m_raw = rwNode->m_raw;
	//	//	casted->m_rawArray.CopyDataFrom(rwNode->m_raw->toArray());
	//	//}
	//}
	CJsonRwNode* CastJsonRwNode(CRawRwNode* base)
	{
		ASSERT(dynamic_cast<CJsonRwNode*>(base) != NULL);
		return static_cast<CJsonRwNode*>(base);
	}
}

namespace TreeReadWrite2
{
	static void TestInternalJsonTreeReadWrite()
	{
		{
			using namespace TreeReadWrite2;
			auto MySubFunc = [](CRwNode& rwRoot)
			{
				AddRwInt32(rwRoot, "delta", 123);
			};
			auto MyFunc = [&MySubFunc](CRwNode& rwRoot)
			{
				AddRwBool(rwRoot, "zeta", false);
				AddRwString(rwRoot, "gama", "test string");
				MySubFunc(rwRoot);

				auto rwAlpha = AddRwNode(rwRoot, "alpha");
				AddRwInt32(rwAlpha, "nihao", 999);
				auto rwArray = AddRwArray(rwAlpha, "arr");
				rwArray->AddItemInt32(345);
				rwArray->AddItemInt32(6789);

				auto rwValue = FindRwValue(rwRoot, "stars");
				if (rwValue != NULL)
					printf("starts found\n");
				else
					printf("starts not found\n");

				// 2. Modify it by DOM.
				auto rwS = rwValue;
				if (rwS == NULL)
					rwS = AddRwValue(rwRoot, "stars");
				rwS->SetInt32(rwS->GetInt32(0) + 1);
				
				rwS = FindRwValue(rwRoot, "project");
				if (rwS != NULL)
				{
					rwS->SetString(rwS->GetString(Niflect::CString()) + " is a piece of shit");
				}
			};

			//auto MyFunc2 = [&MySubFunc](const CRwArray& rwRoot)
			//{
			//	auto count = GetRwItemsCount(rwRoot);
			//	for (uint32 idx = 0; idx < count; ++idx)
			//	{
			//		auto rwValue = rwRoot->GetItemValue(idx);
			//		rwValue->SetString(CStaticFunc::Printf("%s, %u", rwValue->GetString().c_str(), idx));
			//	}
			//};

			//begin, д

			const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
		
			const char* expectedResult = R"({
    "alpha": {
        "arr": [
            345,
            6789
        ],
        "nihao": 999
    },
    "delta": 123,
    "gama": "test string",
    "project": "rapidjson is a piece of shit",
    "stars": 11,
    "zeta": false
})";

			const char* json2 = R"(
[
	"rapidjson",
	"stars"
]
	)";
			//��Ϊallocator�ĵ����÷�, �����ȫ����allocator��Document��StringBuffer�����������Ե�allocator, ���allocatorΪ���ͷŵ�Pool��Ĭ��ռ���ڴ�ԼΪ65536*3
			CRwJson::SizeType poolCapa = 0;//Pool�Ŀ��ڴ�
			CRwJson::SizeType stackCapa = 0;//Json�ڵ���Object��Ԥ���ڴ�
			//CRwJson::AllocatorStack stack;
			//CRwJson::AllocatorPool allocator(poolCapa, &stack);
			CRwJson::AllocatorStack stack;
			auto& allocator = stack;

			CRwJson::Document jd(&allocator, poolCapa, &allocator);

			CRwJson::ReadFromBuffer(jd, json);
#undef GetObject
			auto joRoot = jd.GetObject();
			CJsonRwDocument doc(jd, joRoot);
		
			auto rwRoot = doc.GetRootNode();
			MyFunc(rwRoot);
			//auto rwRootArray = doc.GetRootArray();
			//MyFunc2(rwRootArray);

			CRwJson::StringBuffer buffer(&allocator, poolCapa);
		
			CRwJson::WriteToBuffer(joRoot, buffer);
			printf("%s\n", buffer.GetString());

			ASSERT(buffer.GetLength() == strlen(expectedResult));
			ASSERT(memcmp(buffer.GetString(), expectedResult, buffer.GetSize()) == 0);//�ѷ�������json, �ò��Կɲ�ִ��

			//std::ofstream ofs;
			//ofs.open("E:/a.txt", std::ios::binary);
			//ofs << buffer.GetString();
			//ofs.close();
		}
		printf("");
	}
}

///////////////////////////// ����Ϊ��ʾ��, �������� ///////////////////////////////////

//#ifdef WIN32
//	AllocConsole();
//	freopen("CONOUT$", "w", stdout);
//#endif
//	{
//		//begin, ��
//		StreamReadWrite::CFileStream fs;
//		ref_ptr<CMemoryDataStream> ms;
//		if (fs.OpenRead("shaderex/shader_editor/test.sea"))
//		{
//			auto fileSize = fs.GetSize();
//			ms = CreateMemoryDataStream(static_cast<uint32>(fileSize), true);
//			fs.Read(ms->GetPtr(), fileSize);
//		}
//		CRwJson::Document jd;
//		CRwJson::Object joPar = CRwJson::ReadFromBuffer(jd, ms->GetPtr());
//		CRwJson::Object::Array jaConn = joPar["Connections"].toArray();
//		for (auto& it : jaConn)
//		{
//			auto joConn = it.toObject();
//			printf("nodeFrom: %s\n", joConn["nodeFrom"].toString().c_str());
//			printf("nodeTo: %s\n", joConn["nodeTo"].toString().c_str());
//			printf("portIdxFrom: %d\n", joConn["portIdxFrom"].toInt());
//			printf("portIdxTo: %d\n", joConn["portIdxTo"].toInt());
//		}
//		CRwJson::StringBuffer buffer;
//		CRwJson::WriteToBuffer(jd, buffer);
//		std::cout << buffer.GetString() << std::endl;
//		//end
//	}
//
//	{
//
//		//begin, д
//		const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
//		CRwJson::Document jd;
//		CRwJson::Object joPar = CRwJson::ReadFromBuffer(jd, json);//CRwJson::Object joPar(jd);//
//
//		joPar["zeta"] = false;
//		joPar["gama"] = "test string";
//		joPar["delta"] = 123;
//
//		CRwJson::Object v(jd);
//		v["nihao"] = 999;
//		CRwJson::Object arr(jd);
//		arr.push_back(345);
//		arr.push_back(6789);
//		v["arr"] = arr;
//		joPar["alpha"] = v;
//
//		const auto& refPar = joPar;
//		auto itFound = refPar.find("stars");
//		if (itFound != refPar.end())
//			printf("starts found\n");
//		else
//			printf("starts not found\n");
//
//		// 2. Modify it by DOM.
//		auto s = joPar["stars"];
//		if (!s.isEmpty())
//		{
//			joPar["stars"] = s.GetInt() + 1;
//		}
//
//		s = joPar["project"];
//		if (!s.isEmpty())
//		{
//			joPar["project"] = s.toString() + " is a piece of shit";
//		}
//
//		CRwJson::StringBuffer buffer;
//		CRwJson::WriteToBuffer(joPar, buffer);
//		std::cout << buffer.GetString() << std::endl;
//		//end
//		std::ofstream ofs;
//		ofs.open("E:/a.txt", std::ios::binary);
//		ofs << buffer.GetString();
//		ofs.close();
//	}

//#include "Graph/Base/Util/ReadWrite/Tree/JsonTreeReadWrite.h"
//#include "Graph/Base/Util/ReadWrite/Stream/FileStream.h"
//#include "Graph/Base/Util/ReadWrite/Stream/MemoryStream.h"
//#include "Graph/Base/Util/TempStackMemory.h"
	//void MyExp1()
	//{
	//#ifdef WIN32
	//	AllocConsole();
	//	freopen("CONOUT$", "w", stdout);
	//#endif
	//	using namespace TreeReadWrite;
	//	using namespace StreamReadWrite;

	//	CMemoryStreamDefault msTest;
	//	{
	//		CFileStream fs;
	//		if (fs.Open("F:/Meshlab/512x512_Texel_Density_Texture_1.png", true))
	//		{
	//			CReader reader(&fs);
	//			CWriter writer(&msTest);
	//			CTempStackMemory_1KB temp;
	//			CopySkip(reader, writer, temp, reader.GetSize());
	//		}

	//		//#define TEST_RAW_DATA "haikel124kljlkadf s9f -103j raklsd; ;asl`j;j fah4315h23u.adf adfh;;;;kads;fj o	jw jk;lasdj fljasdf1ij3 fas f/cxj zkljewf;sdf;afajdfioqiej123456789abcde"
	//		////#define TEST_RAW_DATA "haike"
	//		//			msTest.Write(TEST_RAW_DATA, strlen(TEST_RAW_DATA));
	//	}

	//	{

	//		CRwJson::Document jd;
	//		auto& joRoot = jd.toObject();
	//		CJsonRwDocument doc(jd, joRoot);

	//		auto& rwRoot = doc.GetRootNode();
	//		AddRwString(rwRoot, "nihao", "bucuo");
	//		AddRwRawData(rwRoot, "shima", msTest.GetData(), static_cast<size_t>(msTest.GetSize()));

	//		CRwJson::StringBuffer buffer;
	//		CRwJson::WriteToBuffer(joRoot, buffer);
	//		std::string strData = buffer.GetString();
	//		std::cout << strData << std::endl;

	//		{
	//			CFileStream fs;
	//			if (fs.Open("E:/a.txt", false))
	//			{
	//				CWriter writer(&fs);
	//				writer.Write(buffer.GetString(), buffer.GetSize());
	//			}
	//		}
	//	}

	//	{
	//		using namespace StreamReadWrite;
	//		CFileStream fs;
	//		CMemoryStreamDefault ms;
	//		if (fs.Open("E:/a.txt", true))
	//		{
	//			CReader reader(&fs);
	//			CWriter writer(&ms);
	//			CTempStackMemory_1KB temp;
	//			CopySkip(reader, writer, temp, reader.GetSize());
	//			const char ending = '\0';
	//			writer.Write(&ending, sizeof(ending));
	//		}

	//		CRwJson::Document jd;
	//		auto& joRoot = CRwJson::ReadFromBuffer(jd, ms.GetData());
	//		CJsonRwDocument doc(jd, joRoot);

	//		auto& rwRoot = doc.GetRootNode();
	//		auto a = GetRwString(rwRoot, "nihao");
	//		auto rawData = GetRwRawData(rwRoot, "shima");
	//		if (rawData.m_data == NULL)
	//			ASSERT(false);
	//		std::string strRead;
	//		strRead.resize(rawData.m_size);
	//		memcpy(&strRead[0], rawData.m_data, rawData.m_size);
	//		ASSERT(msTest.GetSize() == rawData.m_size);
	//		ASSERT(memcmp(&strRead[0], msTest.GetData(), static_cast<size_t>(msTest.GetSize())) == 0);
	//		printf("");
	//	}
	//}