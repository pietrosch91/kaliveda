Int_t SETUP()
{
   if (gSystem->Load("libKVMultiDet") == -1)
      return -1;
   KVBase::InitEnvironment();
   return 0;
}
