import * as React from "react"
import PageContainer from "../shared/PageContainer"
import { Monster3DProfile, ActionType } from "react-monstereos-profile"


import get3dModel from "../monsters/monster3DMatrix"

import {
  monsterModelSrc,
} from "../monsters/monsters"
import MessageBoard from "../shared/MessageBoard"
import MessageSenderModal from "../shared/MessageSenderModal"

const PAGE_WELCOME = process.env.REACT_APP_PAGE_WELCOME || "A Monster Tamagotchi and Battle game for EOS blockchain!"

interface Props {
  scatter:any,
  identity:any
}

interface ReactState {
  showMessageSender:boolean
}

class HomeScreen extends React.Component<Props, ReactState> {
  public state = {showMessageSender:false}

  public render() {
    const {showMessageSender} = this.state

  const monster3dModel = get3dModel(105) // DEVIL

  const messageSenderClosed = () => {
    this.setState({showMessageSender:false})
  }

  const doShowMessageSender = () => {
    this.setState({showMessageSender:true})
  }


  return (<PageContainer>
    <div>
      <Monster3DProfile
          typeId={"105"}
          path={monsterModelSrc(monster3dModel.model)}
          action={ActionType.IDLE}
          {...monster3dModel}
          size={{ height: "350px" }}
          background={{ alpha: 0 }}
          zoom={false}
        />
    </div>
    <p className="home-monster">{PAGE_WELCOME}</p>
    <MessageBoard/>
    <p onClick={doShowMessageSender}>Send a message</p>                      
    {showMessageSender && <MessageSenderModal
      closeModal = {messageSenderClosed} 
    />}
  </PageContainer>)
  }
}

export default HomeScreen