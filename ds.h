
struct msg
{
    char Pname[1000];
    char data[512];
    struct msg *next;
};
struct Subscriber
{
    char Name[1000];
    int NumofTopics;
    char TopicList[100][1000];
    struct Subscriber *next;
    struct msg *LastRead[100];
};
struct Publisher
{
    int NumberofTopics;
    char Pname[1000];
    char TopicsList[100][1000];
    struct Publisher *next;
};
struct Topic 
{
    char Name[1000];
    // char Creater[1000];
    struct msg *fmsgptr;
    struct msg *lmsgptr;
    struct Topic *next;
};

void printAllPublisher(struct Publisher *temp)
{
    
    while(temp!=NULL)
    {
        printf("Publisher name:%s:\n",temp->Pname);
        for(int i=0;i<temp->NumberofTopics;i++)
        {
            printf("\t Topic :%s:\n",temp->TopicsList[i]);
        }
        temp = temp->next;
    }
}
void printAllTopic(struct Topic *temp)
{
    
    while(temp!=NULL)
    {
        printf("Topics name:%s:\n",temp->Name);
        struct msg *new = temp->fmsgptr;
        while(new!=NULL)
        {
            printf("\tmsg :%s:\n",new->data);
            new= new->next;
        }
        temp = temp->next;
    }
}
void printAllSubscriber(struct Subscriber *sub)
{
    while (sub!=NULL)
    {
        printf("Subscriber : %s\n",sub->Name);
        for(int i=0;i<sub->NumofTopics;i++)
        {
            printf("\tTopics %s\n",sub->TopicList[i]);
        }
        sub = sub->next;
    }
    
}
struct Topic* addTopic(struct Topic *temp,char *topic,char *Pname)
{
    if(temp==NULL)
    {
        temp = (struct Topic *)malloc(sizeof(struct Topic));
        strcpy(temp->Name,topic);
        temp->fmsgptr = NULL;
        temp->lmsgptr = NULL;
        temp->next = NULL;
        return temp;
    }
    else
    {
        int flag=0;
        struct Topic *iter = temp;
        struct Topic *prev = temp;
        while(iter!=NULL)
        {
            if(strcmp(iter->Name,topic)==0)
            {
                flag=1;
                break;
            }
            prev=iter;
            iter = iter->next;
        }
        if(!flag)
        {
            struct Topic *NewTopic = (struct Topic *)malloc(sizeof(struct Topic));
            strcpy(NewTopic->Name,topic);
            NewTopic->fmsgptr = NULL;
            NewTopic->lmsgptr = NULL;
            NewTopic->next = NULL;
            struct Topic *prev = temp;
            while(prev->next!=NULL)
            {
                prev = prev->next;
            }
            prev->next = NewTopic;
        }
        return temp;
    }
}

void addMsgInTopic(struct Topic *temp,struct msg *NewMsg)
{
    struct msg *prevmsg = temp->fmsgptr;
     if(prevmsg==NULL)
    {
        prevmsg = NewMsg;
        temp->fmsgptr = prevmsg;
    }
    else
    {
        while(prevmsg->next!=NULL)
        {
            prevmsg = prevmsg->next;
        }
        prevmsg->next = NewMsg;
        return;
    }
}
void addMsg(struct Topic *temp,char *Msg,char *Pname,char *topic)
{
    if(temp==NULL)
    {
        printf("1No such Topic exists:\n");
        return;
    }
    else
    {
        int flag=0;
        struct msg *NewMsg = (struct msg *)malloc(sizeof(struct msg));
        strcpy(NewMsg->data,Msg);
        strcpy(NewMsg->Pname,Pname);
        NewMsg->next = NULL;
        struct Topic *prev = temp;
        while(prev!=NULL)
        {
            // printf("Comapring :%s: with :%s: %d\n",topic,prev->Name,strcmp(prev->Name,topic));
            if(strcmp(prev->Name,topic)==0)
            {
                flag=1;
                break;
            }
            else
            {
                prev = prev->next;
            }
        }
        if(!flag)
        {
            printf("No such Topic exists:\n");
            return;
        }
        else
        {
            addMsgInTopic(prev,NewMsg);
            prev->lmsgptr = NewMsg;
            printf("New msg is added:\n");
            return;
        }
        return;
    }
}
struct Publisher * addPublisher(struct Publisher *temp,char *TopicName,char *Pname)
{
    if(temp==NULL)
    {
        temp = (struct Publisher *)malloc(sizeof(struct Publisher));
        strcpy(temp->Pname,Pname);
        temp->next = NULL;
        temp->NumberofTopics=1;
        strcpy(temp->TopicsList[0],TopicName);
        return temp;
    }
    else
    {
        // printf("Publihser PublisherList is not empty\n");
        struct Publisher *iter = temp;
        struct Publisher *prev = NULL;
        int flag=0;
        while(iter!=NULL)
        {
            // printf("Publihser found %d\n",strcmp(iter->Pname,Pname));
            if(strcmp(iter->Pname,Pname)==0)
            {
                // printf("Publihser found\n");
                flag=1;
                break;
            }
            prev = iter;
            iter = iter->next;
        }
        if(flag)  //publisher exists
        {
            flag=0;
            int i=0;
                // printf("looking for topics found\n");
            for(i=0;i<iter->NumberofTopics;i++)
            {
                if(strcmp(iter->TopicsList[i],TopicName)==0)
                {
                    printf("Toic already exists for topics found\n");
                    flag=1;
                    break;
                }
            }
            if(!flag) // new topic
            {
                strcpy(iter->TopicsList[i],TopicName);
                iter->NumberofTopics++;
            }
            return temp;
        }
        else // publisher do not exists
        {
            struct Publisher *NewPub = (struct Publisher *)malloc(sizeof(struct Publisher));
            strcpy(NewPub->Pname,Pname);
            NewPub->next = NULL;
            NewPub->NumberofTopics=1;
            strcpy(NewPub->TopicsList[0],TopicName);
            prev->next = NewPub;
            return temp;
        }
    }
}
int matchTopicPublisher(struct Publisher *PList,char *topic, char *Pname)
{
    if(PList == NULL)
    {
        // printf("No topics Named %s found\n",topic);   
        return 0;
    }
    else
    {
        int flag =0;
        while(PList!=NULL)
        {
            if(strncmp(PList->Pname,Pname,strlen(Pname))==0)
            {
                flag=1;
                break;
            }
            PList = PList->next;
        }
        if(flag)
        {   
            // printf("return Plist of %s\n",PList->Pname);
            flag=0;
            for(int i=0;i<PList->NumberofTopics;i++)
            {
                // printf("Comparing :%s: with :%s: %d\n",PList->TopicsList[i],topic,strcmp(PList->TopicsList[i],topic));
                if(strcmp(PList->TopicsList[i],topic)==0)
                {
                    flag=1;
                    break;
                }
            }
            if(!flag)
                return 0;
            else return 1;
        }
        else return 0;
    }
}
int TopicExists(struct Topic *temp,char *topic)
{
    while(temp!=NULL)
    {
        if(strcmp(temp->Name,topic)==0)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
struct Subscriber * addSbscr(struct Subscriber* temp,struct Topic *TList,char *topic, char *Sname,int *result,char *err)
{
    *result=1;
    strcpy(err,"");
    if(TopicExists(TList,topic))
    {
        printf("topic exists\n");
        struct Subscriber *iter = temp;
        struct Subscriber *prev = NULL;
        while(iter!=NULL)
        {
            if(strcmp(iter->Name,Sname)==0)
            {
                int i=0;
                for(;i<iter->NumofTopics;i++)
                {
                    if(strcmp(iter->TopicList[i],topic)==0)
                        return temp;
                }
                iter->NumofTopics++;
                strcpy(iter->TopicList[i],topic);
                iter->LastRead[i] = NULL;
                return temp;
            }
            prev = iter;
            iter = iter->next;
        }
        iter = (struct Subscriber *)malloc(sizeof(struct Subscriber));
        strcpy(iter->Name,Sname);
        strcpy(iter->TopicList[0],topic);
        iter->NumofTopics = 1;
        iter->LastRead[0]=NULL;
        iter->next=NULL;
        if(prev==NULL)
            return iter;
        prev->next=iter;
        return temp;
    }
    else
    {
        *result=0;
        strcpy(err,"No such topic found");
        return temp;
    }
    
}
int isSubscbr(struct Subscriber *temp,char *Sname)
{
    while(temp!=NULL)
    {
        if(strcmp(temp->Name,Sname)==0)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
int isTopicSubscbr(struct Subscriber *temp,char *Sname,char *topicname)
{
    while(temp!=NULL)
    {
        if(strcmp(temp->Name,Sname)==0)
        {
            for(int i=0;i<temp->NumofTopics;i++)
            {
                if(strcmp(temp->TopicList[i],topicname)==0)
                {
                    return 1;
                }
            }
            return 0;
        }
        temp=temp->next;
    }
    return 0;
}
struct msg * GetNextMsg(struct Topic *Tlist,char *tname,struct msg *ToRead)
{
    if(ToRead==NULL)
    {
        printf("Searchinf for topuic\n");
        while(Tlist!=NULL)
        {
            if(strcmp(Tlist->Name,tname)==0)
            {
                printf("Topic dounf %s\n",Tlist->Name);
                return Tlist->fmsgptr;
            }
            Tlist = Tlist->next;
        }
        return NULL;
    }
    else
    {
        return ToRead->next;
    }
    
}

char * GetMsg(struct Topic *Tlist,struct Subscriber *SubList,char *tname, char *Sname)
{   
    // printf("start\n");
    while(SubList!=NULL)
    {
        // printf("start1\n");
        if(strcmp(SubList->Name,Sname)==0)
        {   
            // printf("Subscription found %u\n");
            for(int i=0;i<SubList->NumofTopics;i++)
            {
                if(strcmp(SubList->TopicList[i],tname)==0)
                {
                    // printf("Topic found %u\n",SubList->TopicList[i]);
                    SubList->LastRead[i] = GetNextMsg(Tlist,tname,SubList->LastRead[i]);
                    if(SubList->LastRead[i]==NULL)
                        return "::No further Msgs Next. Further request will repeat the msgs";
                    else 
                        return SubList->LastRead[i]->data;
                }
            }
        }
        SubList = SubList->next;
    }
}
void Listoftopic(struct Topic *Tlist,char *data)
{
    bzero(data,1000);
    while(Tlist!=NULL)
    {
        strcat(data,Tlist->Name);
        strcat(data,"\n");
        Tlist=Tlist->next;
    }    
}