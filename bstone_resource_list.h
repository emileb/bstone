#ifndef BSTONE_RESOURCE_LIST_H
#define BSTONE_RESOURCE_LIST_H


#include <map>


namespace bstone {


template<class T>
class ResourceList {
public:
    ResourceList()
    {
    }

    ~ResourceList()
    {
        clear();
    }

    T* add(
        int id)
    {
        T* resource = get(id);

        if (resource != NULL)
            return resource;

        resource = new T(id);

        list_[id] = resource;

        return resource;
    }

    bool remove(
        int id)
    {
        ListCIt it = list_.find(id);

        if (it == list_.end())
            return false;

        delete it->second;

        list_.remove(it);

        return true;
    }

    T* get(
        int id) const
    {
        ListCIt it = list_.find(id);

        if (it != list_.end())
            return it->second;

        return NULL;
    }

    void clear()
    {
        for (ListCIt i = list_.begin(); i != list_.end(); ++i)
            delete i->second;

        list_.clear();
    }

private:
    typedef std::map<int,T*> List;
    typedef typename List::iterator ListIt;
    typedef typename List::const_iterator ListCIt;

    List list_;

    ResourceList(
        const ResourceList& that);

    ResourceList& operator=(
        const ResourceList& that);
}; // class ResourceList


} // namespace bstone


#endif // BSTONE_RESOURCE_LIST_H
